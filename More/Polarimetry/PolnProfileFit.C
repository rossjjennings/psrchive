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
#include "Pauli.h"
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

  maximum_harmonic = n_harmonic = 0;
}

void Pulsar::PolnProfileFit::set_maximum_harmonic (unsigned max)
{
  maximum_harmonic = max;
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

  standard_variance = get_variance (standard);
  standard_fourier = fourier_transform (standard);
  standard_power = standard_fourier->sumsq (1);

  // number of complex phase bins in Fourier domain
  unsigned std_harmonic = standard->get_nbin() / 2;

  if (maximum_harmonic && maximum_harmonic < std_harmonic) {
    //if (Profile::verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard using " << maximum_harmonic
	   << " out of " << std_harmonic << " harmonics" << endl;
    n_harmonic = maximum_harmonic;
  }
  else {
    //if (Profile::verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard using " << std_harmonic 
	   << " harmonics" << endl;
    n_harmonic = std_harmonic;
  }


  model = new Calibration::ReceptionModel;

  // initialize the model transformation
  model->add_transformation ();
  if (transformation)
    model->set_transformation (transformation);

  unsigned npol = 4;

  // initialize the model input states
  for (unsigned ibin=1; ibin<n_harmonic; ibin++) {

    Stokes< complex<double> > stokes;

    for (unsigned ipol=0; ipol<npol; ipol++) {
      const float* amps = standard_fourier->get_amps(ipol) + ibin*2;
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
void Pulsar::PolnProfileFit::fit (const PolnProfile* observation) try
{
  if (!standard)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "no standard specified.  call set_standard");

  if (!transformation)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "no transformation specified.  call set_transformation");

  unsigned obs_harmonic = observation->get_nbin() / 2;

  if (obs_harmonic < n_harmonic)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "observation n_harmonic=%d < n_harmonic=%d",
		 obs_harmonic, n_harmonic);

  Reference::To<PolnProfile> fourier = fourier_transform (observation);

  float phase_guess = ccf_max_phase (standard_fourier->get_Profile(0),
				     fourier->get_Profile(0));

  set_phase (phase_guess);

  Stokes<float> variance = get_variance (observation);
  double power = fourier->sumsq (1);
  double gain = power / standard_power;

  unsigned npol = 4;
  for (unsigned ipol=0; ipol<npol; ipol++) {
    // the Fourier transform will inflate the variance
    variance[ipol] *= observation->get_nbin();
    // and the noise in the standard will contribute
    variance[ipol] += gain * standard_variance[ipol] * standard->get_nbin();
  }

    // variance[ipol] *= nbin;

  gain = sqrt(gain);
  cerr << "gain=" << gain << endl;

  // calculate the rms in the baseline of each profile


  model->delete_data ();

  // initialize the measurement sets
  for (unsigned ibin=1; ibin<n_harmonic; ibin++) {

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
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::fit";
}

//! Get the phase offset between the observation and the standard
Estimate<double> Pulsar::PolnProfileFit::get_phase () const
{
  return phase -> get_Estimate (1);
}

//! Get the phase offset between the observation and the standard
void Pulsar::PolnProfileFit::set_phase (const Estimate<double>& value)
{
  phase -> set_Estimate (1, value);
}

/*!
  Calculates the shift between
  Returns a basic Tempo::toa object
*/
Tempo::toa Pulsar::PolnProfileFit::get_toa (const PolnProfile* observation,
					    const MJD& mjd, 
					    double period, char nsite) try 
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
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::get_toa";
}

Pulsar::PolnProfile* 
Pulsar::PolnProfileFit::fourier_transform (const PolnProfile* input) const try
{
  Reference::To<PolnProfile> fourier;

  fourier = new PolnProfile (input->get_basis(), input->get_state(),
			     new Profile, new Profile,
			     new Profile, new Profile);

  unsigned nbin = input->get_nbin();
  fourier->resize( nbin );

  unsigned npol = 4;
  for (unsigned ipol=0; ipol<npol; ipol++)
    fft::frc1d (nbin, fourier->get_amps(ipol), input->get_amps(ipol));

  fourier->convert_state (Signal::Stokes);

  return fourier.release();
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::fourier_transform";
}

Stokes<float> 
Pulsar::PolnProfileFit::get_variance (const PolnProfile* input) const try
{
  Reference::To<PolnProfile> temp;

  if (input->get_state() != Signal::Stokes) {
    temp = input->clone();
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
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::get_variance";
}

float Pulsar::PolnProfileFit::ccf_max_phase (const Profile* std,
					     const Profile* obs) const try
{
  unsigned ibin, nbin = std->get_nbin();
  if (obs->get_nbin() < nbin)
    nbin = obs->get_nbin();

  // calculate the cross power spectral density
  float* cpsd = new float[nbin];
  auto_ptr<float> delete_cpsd (cpsd);

  // don't care about DC term
  cpsd[0] = cpsd[1] = 0.0;
  for (ibin=2; ibin < nbin; ibin+=2) {

    complex<float> c_std (std->get_amps()[ibin], std->get_amps()[ibin+1]);
    complex<float> c_obs (obs->get_amps()[ibin], obs->get_amps()[ibin+1]);

    complex<float> c_psd = c_obs * conj(c_std);

    cpsd[ibin]   = c_psd.real();
    cpsd[ibin+1] = c_psd.imag();

  }

  // calculate the (complex) cross correlation function
  float* ccf = new float[nbin];
  auto_ptr<float> delete_ccf (ccf);

  nbin /= 2;
  fft::bcc1d (nbin, ccf, cpsd);

  // find the maximum modulus
  float max = 0;
  float imax = 0;
  for (ibin=0; ibin < nbin; ibin++) {
    float mod = ccf[2*ibin]*ccf[2*ibin] + ccf[2*ibin+1]*ccf[2*ibin+1];
    if (mod > max) {
      max = mod;
      imax = ibin;
    }
  }

  if (imax > nbin/2)
    imax -= nbin;

  return float(imax)/nbin;

}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::ccf_max_phase";
}
