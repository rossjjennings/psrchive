#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ExponentialBaseline.h"

#include "Calibration/ReceptionModel.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Phase.h"

#include "MEAL/ChainRule.h"
#include "MEAL/Complex2Math.h"
#include "MEAL/Complex2Constant.h"
#include "Calibration/CoherencyMeasurementSet.h"

#include "RealTimer.h"
#include "Pauli.h"
#include "fftm.h"

// #define _DEBUG 1

#ifdef _DEBUG
#include <cpgplot.h>
#endif

#include <memory>


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
  phase = new MEAL::Polynomial (2);
  phase -> set_param (0, 0.0);
  phase -> set_infit (0, false);

  phase -> set_argument (0, &phase_axis);

  MEAL::ChainRule<MEAL::Complex2>* chain = 0;
  chain = new MEAL::ChainRule<MEAL::Complex2>;
  chain -> set_model (new MEAL::Phase);
  chain -> set_constraint (0, phase);

  phase_xform = chain;

  maximum_harmonic = n_harmonic = 0;

  choose_maximum_harmonic = false;

  var_phase = 0.0;
  iterations = 0;
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


  standard_fourier = fourier_transform (standard);
  set_noise_mask ();

  // don't include DC in sum
  standard_power = standard_fourier->sumsq (1);

  // number of complex phase bins in Fourier domain
  unsigned std_harmonic = standard->get_nbin() / 2;

  if (choose_maximum_harmonic) {
    //if (Profile::verbose)
    cerr << "Pulsar::PolnProfileFit::set_standard chose "
	 << n_harmonic << " harmonics" << endl;
  }
  else if (maximum_harmonic && maximum_harmonic < std_harmonic) {
    //if (Profile::verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard using " << maximum_harmonic
	   << " out of " << std_harmonic << " harmonics" << endl;
    n_harmonic = maximum_harmonic;
  }
  else {
    //if (Profile::verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard using all "
	   << std_harmonic << " harmonics" << endl;
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
    MEAL::Complex2Constant* jones;
    jones = new MEAL::Complex2Constant( convert(stokes) );

#ifdef _DEBUG
    cerr << "Pulsar::PolnProfileFit::set_standard ibin=" << ibin 
	 << "\n  " << stokes << endl;
#endif

    // each complex phase bin is phase related
    Reference::To<MEAL::Complex2> input = jones;

    model->add_input( input * phase_xform );

  }

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileFit::set_standard exit" << endl;

}

//! Choose the maximum_harmonic for the given standard
void Pulsar::PolnProfileFit::choose_max_harmonic (const PolnProfile* psd)
{
  n_harmonic = psd->get_nbin();

  unsigned max_harmonic = 0;
  unsigned npol = 4;
  unsigned count = 0;

  for (unsigned ipol=0; ipol<npol; ipol++) {

    const float* amps = psd->get_amps(ipol);
    double threshold = standard_variance[ipol] * 3.0;

    // cerr << "THRESHOLD=" << threshold  << endl;

    for (unsigned ibin=1; ibin<n_harmonic; ibin++) {

      if (amps[ibin] > threshold)
	count ++;
      else
	count = 0;

      // cerr << count << " " << ibin << " " << amps[ibin]  <<endl;

      if (count > 3)
	max_harmonic = ibin;

    }
  }

  n_harmonic = max_harmonic;
}

//! Set the transformation between the standard and observation
void 
Pulsar::PolnProfileFit::set_transformation (MEAL::Complex2* xform)
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

  Stokes<float> variance = get_variance (fourier);

  // don't include DC in sum
  double power = fourier->sumsq (1);
  double gain = power / standard_power;

  cerr << "FIX THE GAIN=" << gain << endl;
  // The gain calculation needs to be re-considered
  gain = 0.0;

  unsigned npol = 4;
  for (unsigned ipol=0; ipol<npol; ipol++) {
    // the noise in the standard will contribute
    variance[ipol] += gain * standard_variance[ipol];
    variance[ipol] *= sqrt(.75);
  }

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

#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFit::fit solved in " << clock << endl;
#endif

  vector< vector<double> > covariance;
  model->get_fit_covariance (covariance);

  Matrix <7,7,double> c_Jones;       // C_J in str05
  Vector <7,double> c_phase_Jones;   // c_{\varphi J} in str05

  unsigned i, j;

  fprintf (stderr, "%12s%12.2g\n", "phase", covariance[2][2]);

  for (i=0; i < 7; i++) {

    unsigned m = i + 3;
    fprintf (stderr, "%12s", model->get_param_name(m).c_str());

    c_phase_Jones[i] = covariance[m][2];
    // c_phase_Jones[i] /= sqrt(covariance[m][m]*covariance[2][2]);

    fprintf (stderr, "%12.2g", c_phase_Jones[i]);

    for (unsigned j=0; j < 7; j++) {

      unsigned n = j + 3;
      c_Jones[i][j] = covariance[m][n];
      // c_Jones[i][j] /= sqrt(covariance[m][m]*covariance[n][n]);

      if (j > i)
	continue;

      fprintf (stderr, "%12.2g", c_Jones[i][j]);

    }

    fprintf (stderr, "\n");
  }

  var_phase += covariance[2][2];
  cov_Jones += c_Jones;
  cov_phase_Jones += c_phase_Jones;
  iterations ++;

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
Pulsar::PolnProfileFit::fourier_psd (const PolnProfile* fourier) const try
{
  Reference::To<PolnProfile> psd;

  psd = new PolnProfile (fourier->get_basis(), fourier->get_state(),
			 new Profile, new Profile,
			 new Profile, new Profile);

  unsigned nbin = fourier->get_nbin()/2;
  psd->resize( nbin );

  unsigned npol = 4;
  for (unsigned ipol=0; ipol<npol; ipol++)  {
    const float* in = fourier->get_Profile(ipol)->get_amps();
    float* out = psd->get_Profile(ipol)->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++) {
      float re = in[ibin*2];
      float im = in[ibin*2+1];
      out[ibin] = re*re + im*im;
    }
  }

  return psd.release();
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::fourier_psd";
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
  auto_ptr<float> amps( new float[nbin+2] );

  for (unsigned ipol=0; ipol<npol; ipol++)  {
    fft::frc1d (nbin, amps.get(), input->get_amps(ipol));
    fourier->set_amps(ipol, amps.get());
  }

  fourier->convert_state (Signal::Stokes);

  return fourier.release();
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::fourier_transform";
}

void Pulsar::PolnProfileFit::set_noise_mask () try
{
  // calculate the power spectral density of the input
  Reference::To<PolnProfile> input_psd;
  input_psd = fourier_psd (standard_fourier);

  // initialize the mask
  PhaseWeight mask;
  ExponentialBaseline finder;

  unsigned npol = 4;

  for (unsigned ipol=0; ipol < npol; ipol++) {
    finder.set_Profile (input_psd->get_Profile(0));
    finder.get_weight (mask);
    if (ipol == 0)
      noise_mask = mask;
    else
      noise_mask *= mask;
  }

  standard_variance = get_variance (standard_fourier);

  // while we have the PSD ...
  if (choose_maximum_harmonic)
    choose_max_harmonic (input_psd);

}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::set_noise_mask";
}

Stokes<float> 
Pulsar::PolnProfileFit::get_variance (const PolnProfile* input) const try
{
  Stokes<float> variance;

  unsigned nbin = input->get_nbin()/2;
  unsigned mbin = std::min(nbin, noise_mask.get_nbin());
  unsigned ibin = 0;
  unsigned npol = 4;

  for (unsigned ipol=0; ipol<npol; ipol++) {

    const float* amps = input->get_amps(ipol);
    unsigned count = 0;
    double mean = 0;

    for (ibin = 0; ibin < nbin; ibin++)
      if (ibin > mbin || noise_mask[ibin]) {
	float re = amps[ibin*2];
	float im = amps[ibin*2+1];
	mean += re*re + im*im;
	count ++;
      }

    mean /= count;

    // The variance of the spectrum (with zero mean) is the mean of the PSD
    variance[ipol] = mean;

    cerr << "Pulsar::PolnProfileFit::get_variance ipol=" << ipol 
	 << " sigma=" << sqrt(variance[ipol]) << endl;

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
