#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

#include "Calibration/ReceptionModel.h"
#include "Calibration/Polynomial.h"
#include "Calibration/Phase.h"

#include "Calibration/ChainRule.h"
#include "Calibration/Complex2Math.h"
#include "Calibration/Complex2Constant.h"
#include "Calibration/CoherencyMeasurementSet.h"

#include "RealTimer.h"
#include "fftm.h"

#ifdef _DEBUG
#include <cpgplot.h>
#endif

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

  phase -> set_argument (0, &phase_axis);

  Calibration::ChainRule<Calibration::Complex2>* chain = 0;
  chain = new Calibration::ChainRule<Calibration::Complex2>;
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

  standard_variance = get_variance (standard);
  Reference::To<PolnProfile> fourier = fourier_transform (standard);
  standard_power = fourier->sumsq (1);

  // number of complex phase bins in Fourier domain
  nbin /= 2;

  model = new Calibration::ReceptionModel;

  // initialize the model transformation
  model->add_transformation ();
  if (transformation)
    model->set_transformation (transformation);

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileFit::set_standard nbin=" << nbin << endl;

  // initialize the model input states
  for (unsigned ibin=1; ibin<nbin; ibin++) {

    Stokes< complex<double> > stokes;

    for (unsigned ipol=0; ipol<npol; ipol++) {
      const float* amps = fourier->get_amps(ipol) + ibin*2;
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
    Reference::To<Calibration::Complex2> input = jones;

    model->add_input( input * phase_xform );

  }

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileFit::set_standard exit" << endl;

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

  // first find the course fit in the time domain
  Reference::To<Profile> clone = observation->get_Profile(0)->clone();
  clone->correlate (standard->get_Profile(0));

#ifdef _DEBUG
  cpgopen("?");
  clone->display();
  cpgsci(2);
  observation->get_Profile(0)->display();
  cpgend();
#endif

  int ibin_max = clone->find_max_bin();

  if (ibin_max > (int)nbin/2)
    ibin_max -= nbin;

  Estimate<double> phase_offset (ibin_max);
  phase_offset /= observation->get_nbin();

  set_phase (phase_offset);

  Stokes<float> variance = get_variance (observation);

  Reference::To<PolnProfile> fourier = fourier_transform (observation);
  double power = fourier->sumsq (1);
  double gain = power / standard_power;

  for (unsigned ipol=0; ipol<npol; ipol++) 
    // the Fourier transform will inflate the variance
    variance[ipol] = (variance[ipol] + gain*standard_variance[ipol]) * nbin;
    // variance[ipol] *= nbin;

  gain = sqrt(gain);
  cerr << "gain=" << gain << endl;

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

    double phase_shift = -2.0 * M_PI * double(ibin);

    Calibration::CoherencyMeasurementSet measurements;
    measurements.add_coordinate ( phase_axis.new_Value(phase_shift) );
    measurements.push_back ( measurement );
 
    model->add_data( measurements );

  }

  RealTimer clock;

  clock.start();
  model->solve_work ();
  clock.stop();

  cerr << "Pulsar::PolnProfileFit::fit solved in " << clock << endl;

}

//! Get the phase offset between the observation and the standard
Estimate<double> Pulsar::PolnProfileFit::get_phase () const
{
  return phase -> get_Estimate (1);
}

//! Get the phase offset between the observation and the standard
void Pulsar::PolnProfileFit::set_phase (Estimate<double>& value)
{
  phase -> set_Estimate (1, value);
}

/*!
  Calculates the shift between
  Returns a basic Tempo::toa object
*/
Tempo::toa Pulsar::PolnProfileFit::get_toa (const PolnProfile* observation,
					    const MJD& mjd, 
					    double period, char nsite)
{
  fit (observation);

  Estimate<double> pulse_phase = get_phase();

  cerr << "pulse_phase=" << pulse_phase << endl;

  Tempo::toa retval (Tempo::toa::Parkes);

  retval.set_frequency (observation->get_Profile(0)->get_centre_frequency());
  retval.set_arrival   (mjd + pulse_phase.val * period);
  retval.set_error     (sqrt(pulse_phase.var) * period * 1e6);

  retval.set_telescope (nsite);

  return retval;
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

Stokes<float> 
Pulsar::PolnProfileFit::get_variance (const PolnProfile* input) const
{
  if (input->get_state() != Signal::Stokes) {
    Reference::To<PolnProfile> temp = input->clone();
    temp->convert_state (Signal::Stokes);
    input = temp;
  }

  Stokes< float > variance;

  float min_phase = input->get_Profile(0)->find_min_phase ();
  unsigned npol = 4;

  for (unsigned ipol=0; ipol<npol; ipol++) {
    double mean, var;
    input->get_Profile(ipol)->stats (min_phase, &mean, &var);
    variance[ipol] = var;
  }

  return variance;
}
