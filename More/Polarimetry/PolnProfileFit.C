#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

#include "Calibration/ReceptionModel.h"
#include "Calibration/Abbreviations.h"
#include "Calibration/Polynomial.h"
#include "Calibration/Phase.h"
#include "Calibration/Complex2Constant.h"
#include "Calibration/CoherencyMeasurementSet.h"

#include "fftm.h"

//! Default constructor
Pulsar::PolnProfileFit::PolnProfileFit ()
{
  init ();
}

//! Copy constructor
Pulsar::PolnProfileFit::PolnProfileFit (const PolnProfileFit& fit)
{
  init ();
  operator = (fit);
}

//! Assignment operator
Pulsar::PolnProfileFit&
Pulsar::PolnProfileFit::operator = (const PolnProfileFit& fit)
{
  return *this;
}

//! Destructor
Pulsar::PolnProfileFit::~PolnProfileFit ()
{
}

void Pulsar::PolnProfileFit::init ()
{
  // create the linear phase relationship transformation
  phase = new Calibration::Polynomial (2);
  phase -> set_param (0, 0.0);
  phase -> set_infit (0, false);

  phase_axis.connect (phase, &Calibration::Polynomial::set_abscissa);

  Calibration::Complex2Chain* chain = new Calibration::Complex2Chain;
  chain -> set_model (new Calibration::Phase);
  chain -> set_constraint (0, phase);

  phase_xform = chain;
}

//! Set the standard to which observations will be fit
void Pulsar::PolnProfileFit::set_standard (const PolnProfile* _standard)
{
  standard = _standard;

  // until greater resize functionality is added to ReceptionModel,
  // best to just delete it and start fresh
  model = 0;
  
  if (!standard)
    return;

  unsigned nbin = standard->get_nbin();
  unsigned npol = 4;

  standard = fourier_transform (standard);

  // number of complex phase bins in Fourier domain
  nbin /= 2;

  model = new Calibration::ReceptionModel;

  // initialize the model transformation
  model->add_transformation ();
  if (transformation)
    model->set_transformation (transformation);

  // initialize the model input states
  for (unsigned ibin=1; ibin<nbin; ibin++) {

    Stokes< complex<double> > stokes;

    for (unsigned ipol=0; ipol<npol; ipol++) {
      const float* amps = standard->get_amps(ipol) + ibin*2;
      stokes[ipol] = complex<double>(amps[0], amps[1]);
    }

    // each complex phase bin of the standard is treated as a known constant
    Calibration::Complex2Constant* jones = new Calibration::Complex2Constant;
    jones->set_Jones( convert(stokes) );

#ifdef _DEBUG
    cerr << "Pulsar::PolnProfileFit::set_standard ibin=" << ibin 
	 << "\n  " << stokes << endl;
#endif

    // each complex phase bin is phase related
    Calibration::Complex2Product* product = new Calibration::Complex2Product;
    *product *= jones;
    *product *= phase_xform;

    model->add_input( product );

  }

}

//! Set the transformation between the standard and observation
void 
Pulsar::PolnProfileFit::set_transformation (Calibration::Complex2* xform)
{
  transformation = xform;
  if (model)
    model->set_transformation (xform);
}

//! Fit the specified observation to the standard
void Pulsar::PolnProfileFit::fit (const PolnProfile* observation)
{
  if (!standard)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "no standard specified.  call set_standard");

  if (!transformation)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "no transformation specified.  call set_transformation");

  unsigned nbin = standard->get_nbin();
  unsigned npol = 4;

  if (observation->get_nbin() != nbin)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "observation nbin=%d != standard nbin=%d",
		 observation->get_nbin(), nbin);

  if (observation->get_state() != Signal::Stokes) {
    cerr << "implement clone" << endl;
    PolnProfile* temp = (PolnProfile*) observation;
    temp->convert_state (Signal::Stokes);
    observation = temp;
  }

  Stokes< float > variance;

  float min_phase = observation->get_Profile(0)->find_min_phase ();

  for (unsigned ipol=0; ipol<npol; ipol++) {
    double mean, var;
    observation->get_Profile(ipol)->stats (min_phase, &mean, &var);
    // the Fourier transform will inflate the variance
    variance[ipol] = var * nbin;
  }


  Reference::To<PolnProfile> fourier = fourier_transform (observation);

  // calculate the rms in the baseline of each profile


  // number of complex phase bins in Fourier domain
  nbin /= 2;

  model->delete_data ();

  // initialize the measurement sets
  for (unsigned ibin=1; ibin<nbin; ibin++) {

    Stokes< complex<double> > stokes;

    for (unsigned ipol=0; ipol<npol; ipol++) {
      const float* amps = fourier->get_amps(ipol) + ibin*2;
      stokes[ipol] = complex<double>(amps[0], amps[1]);
    }

#ifdef _DEBUG
    cerr << "Pulsar::PolnProfileFit::fit ibin=" << ibin 
	 << "\n  " << stokes << endl;
#endif

    Calibration::CoherencyMeasurement measurement (ibin-1);
    measurement.set_stokes (stokes, variance);

    double phase_shift = M_PI*double(ibin)/double(nbin);

    Calibration::CoherencyMeasurementSet measurements;
    measurements.add_coordinate ( phase_axis.new_Value(phase_shift) );
    measurements.push_back ( measurement );
 
    model->add_data( measurements );

  }

  model->solve_work ();

}

//! Get the phase offset between the observation and the standard
Estimate<double> Pulsar::PolnProfileFit::get_phase () const
{
  return phase -> get_Estimate (1);
}


Pulsar::PolnProfile* 
Pulsar::PolnProfileFit::fourier_transform (const PolnProfile* input) const
{
  Reference::To<PolnProfile> fourier;

  fourier = new PolnProfile (input->get_basis(), input->get_state(),
			     new Profile, new Profile,
			     new Profile, new Profile);

  unsigned nbin = input->get_nbin();
  unsigned npol = 4;

  fourier->resize( nbin );

  for (unsigned ipol=0; ipol<npol; ipol++)
    fft::frc1d (nbin, fourier->get_amps(ipol), input->get_amps(ipol));

  fourier->convert_state (Signal::Stokes);

  return fourier.release();
}
