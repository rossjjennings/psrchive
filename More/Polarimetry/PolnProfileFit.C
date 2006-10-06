/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Fourier.h"

#include "Pulsar/ExponentialBaseline.h"

#include "Pulsar/ReceptionModel.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/TotalCovariance.h"

#include "MEAL/Polynomial.h"
#include "MEAL/Phase.h"

#include "MEAL/ChainRule.h"
#include "MEAL/Complex2Math.h"
#include "MEAL/Complex2Constant.h"

#include "RealTimer.h"
#include "Pauli.h"
#include "FTransform.h"

// #define _DEBUG 1

#include <memory>

using namespace std;

unsigned get_last_significant (const Pulsar::PolnProfile* psd,
			       const Stokes<double>& var);

bool Pulsar::PolnProfileFit::verbose = false;

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

  uncertainty = new Calibration::TemplateUncertainty;

  maximum_harmonic = n_harmonic = 0;

  choose_maximum_harmonic = false;
  emulate_scalar = false;
  separate_fits = false;
  fit_debug = false;
}

void Pulsar::PolnProfileFit::set_maximum_harmonic (unsigned max)
{
  maximum_harmonic = max;
}

//! Get the standard to which observations will be fit
const Pulsar::PolnProfile* Pulsar::PolnProfileFit::get_standard () const
{
  return standard; 
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

  Reference::To<PolnProfile> fourier = fourier_transform (standard);
  fourier->convert_state (Signal::Stokes);
  // Drop the Nyquist bin
  fourier->resize( standard->get_nbin() );

  standard_fourier = fourier;

  set_noise_mask ();

  // number of complex phase bins in Fourier domain
  unsigned std_harmonic = standard->get_nbin() / 2;

  if (choose_maximum_harmonic) {
    if (verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard chose "
	   << n_harmonic << " harmonics" << endl;
  }
  else if (maximum_harmonic && maximum_harmonic < std_harmonic) {
    if (verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard using " << maximum_harmonic
	   << " out of " << std_harmonic << " harmonics" << endl;
    n_harmonic = maximum_harmonic;
  }
  else {
    if (verbose)
      cerr << "Pulsar::PolnProfileFit::set_standard using all "
	   << std_harmonic << " harmonics" << endl;
    n_harmonic = std_harmonic;
  }


  model = new Calibration::ReceptionModel;
  model->set_fit_debug( fit_debug );

  // initialize the model transformation
  model->add_transformation ();
  if (transformation)
    model->set_transformation (transformation);

  unsigned npol = 4;
  if (emulate_scalar)
    npol = 1;

  standard_det = 0;

  // initialize the model input states
  for (unsigned ibin=1; ibin<n_harmonic; ibin++) {

    Stokes< complex<double> > stokes;
    Stokes<double> re;
    Stokes<double> im;

    for (unsigned ipol=0; ipol<npol; ipol++) {
      const float* amps = standard_fourier->get_amps(ipol) + ibin*2;
      stokes[ipol] = complex<double>(amps[0], amps[1]);
      re[ipol] = amps[0];
      im[ipol] = amps[1];
    }

    standard_det += re.invariant() + im.invariant();

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

  if (verbose)
    cerr << "Pulsar::PolnProfileFit::set_standard det=" << standard_det <<endl;

}

//! Choose the maximum_harmonic for the given standard
void Pulsar::PolnProfileFit::choose_max_harmonic (const PolnProfile* psd)
{
  n_harmonic = get_last_significant (psd, standard_variance);
}

//! Set the transformation between the standard and observation
void Pulsar::PolnProfileFit::set_transformation (MEAL::Complex2* xform)
{
  transformation = xform;

  if (uncertainty)
    uncertainty->set_transformation (xform);

  if (model)
    model->set_transformation (xform);
}

MEAL::Complex2* Pulsar::PolnProfileFit::get_transformation () const
{
  return transformation;
}

//! Set the error propagation policy
void Pulsar::PolnProfileFit::set_uncertainty
(Calibration::TemplateUncertainty* policy)
{
  uncertainty = policy;

  if (uncertainty && transformation)
    uncertainty->set_transformation (transformation);
}

//! Get the error propagation policy
Calibration::TemplateUncertainty* Pulsar::PolnProfileFit::get_uncertainty ()
{
  return uncertainty;
}

void Pulsar::PolnProfileFit::set_separate_fits (bool flag)
{
  separate_fits = flag;
}

void Pulsar::PolnProfileFit::set_fit_debug (bool flag)
{
  fit_debug = flag;
  if (model)
    model->set_fit_debug(flag);
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

  if (!observation)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "no observation supplied as argument");

  // ensure that the PolnProfile class is cleaned up
  Reference::To<const PolnProfile> obs = observation;

  unsigned obs_harmonic = observation->get_nbin() / 2;

  if (obs_harmonic < n_harmonic)
    throw Error (InvalidState, "Pulsar::PolnProfileFit::fit",
		 "observation n_harmonic=%d < n_harmonic=%d",
		 obs_harmonic, n_harmonic);

  Reference::To<PolnProfile> fourier = fourier_transform (observation);
  fourier->convert_state (Signal::Stokes);

  unsigned nbin_std = standard->get_nbin();
  unsigned nbin_obs = observation->get_nbin();

  float phase_guess = ccf_max_phase (standard_fourier->get_Profile(0),
				     fourier->get_Profile(0));

  set_phase (phase_guess);

  uncertainty->set_template_variance (standard_variance);

  Calibration::TotalCovariance* total;
  total = dynamic_cast<Calibration::TotalCovariance*> (uncertainty.get());

  if (total)
    total->set_covariance (get_covariance( fourier ));
  else {
    Stokes<double> var = get_variance( fourier );
    uncertainty->set_variance (var);

    // calculate the power spectral density of the input
    Reference::To<PolnProfile> psd = fourier_psd (fourier);
    n_harmonic_obs = get_last_significant (psd, var);
    
    if (verbose)
      cerr << "Pulsar::PolnProfileFit::fit last harmonic = "
	   << n_harmonic_obs << endl;
  }

  model->delete_data ();

  unsigned npol = 4;
  if (emulate_scalar)
    npol = 1;

  double observation_det = 0;

  // initialize the measurement sets
  for (unsigned ibin=1; ibin<n_harmonic; ibin++) {

    Stokes< complex<double> > stokes;
    Stokes<double> re;
    Stokes<double> im;

    for (unsigned ipol=0; ipol<npol; ipol++) {
      const float* amps = fourier->get_amps(ipol) + ibin*2;
      re[ipol] = amps[0];
      im[ipol] = amps[1];
      stokes[ipol] = complex<double>(amps[0], amps[1]);
    }

    observation_det += re.invariant() + im.invariant();

#ifdef _DEBUG
    cerr << "Pulsar::PolnProfileFit::fit ibin=" << ibin 
	 << "\n  " << stokes << endl;
#endif

    Calibration::CoherencyMeasurement measurement (ibin-1);
    measurement.set_stokes (stokes, uncertainty);

    double phase_shift = -2.0 * M_PI * double(ibin);

    Calibration::CoherencyMeasurementSet measurements;
    measurements.add_coordinate ( phase_axis.new_Value(phase_shift) );
    measurements.push_back ( measurement );
 
    model->add_data( measurements );

  }

  double Gain = 1.0;

  // with optimized template matching, the determinant could be anything
  if (observation_det > 0)
    Gain = sqrt(observation_det / standard_det);

  // WARNING: this next line assumes that gain is the first free parameter
  // of the Complex2 function passed in set_transformation
  transformation->set_param (0, sqrt(Gain));

  RealTimer clock;

  clock.start();

  if (separate_fits) {

    // first fit only for the transformation
    phase->set_infit (1, false);
    for (unsigned i=0; i<transformation->get_nparam(); i++)
      transformation->set_infit (i, true);

    model->solve_work ();

    // then fit only for the phase
    phase->set_infit (1, true);
    for (unsigned i=0; i<transformation->get_nparam(); i++)
      transformation->set_infit (i, false);

  }

  model->solve_work ();

  clock.stop();

  /* if template and observation have different numbers of bins, there
     is an extra offset equal to the offset between the centres of bin
     0 of each profile */
  if (nbin_std != nbin_obs) {
    double mismatch_shift = 0.5/nbin_std - 0.5/nbin_obs;
    phase->set_param (1, phase->get_param(1) - mismatch_shift);
  }

  if (verbose)
    cerr << "Pulsar::PolnProfileFit::fit solved in " << clock << "."
      " chisq=" << get_fit_chisq() / get_fit_nfree() << endl;

}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::fit";
}

//! The number of iterations in last call to solve method
unsigned Pulsar::PolnProfileFit::get_fit_iterations () const
{
  return model->get_fit_iterations();
}

//! The chi-squared in last call to solve method
float Pulsar::PolnProfileFit::get_fit_chisq () const
{
  return model->get_fit_chisq();
}

//! The number of free parameters in last call to solve method
unsigned Pulsar::PolnProfileFit::get_fit_nfree () const
{
  return model->get_fit_nfree();
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
  if (verbose)
    cerr << "Pulsar::PolnProfileFit::get_toa" << endl;

  fit (observation);
  
  Estimate<double> pulse_phase = get_phase();

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
  Reference::To<PolnProfile> psd = fourier->clone();
  detect (psd);
  return psd.release();
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::fourier_psd";
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

    finder.set_Profile( input_psd->get_Profile(ipol) );
    finder.get_weight( mask );

    if (verbose)
      cerr << "Pulsar::PolnProfileFit::set_noise_mask ipol=" << ipol
	   << " mask count=" << mask.get_weight_sum() << endl;

    if (ipol == 0)
      noise_mask = mask;
    else
      noise_mask *= mask;

  }

  standard_variance = get_variance (standard_fourier);
  uncertainty->set_template_variance (standard_variance);

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

  unsigned npol = 4;

  for (unsigned ipol=0; ipol<npol; ipol++) {

    // The variance of the spectrum (with zero mean) is the mean of the PSD
    variance[ipol] = get_variance ( input->get_Profile(ipol) );

    if (verbose)
      cerr << "Pulsar::PolnProfileFit::get_variance ipol=" << ipol 
	   << " sigma=" << sqrt(variance[ipol]) << endl;
  }

  return variance;
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::get_variance";
}

double Pulsar::PolnProfileFit::get_variance (const Profile* input) const try
{
  unsigned nbin = input->get_nbin()/2;
  unsigned mbin = std::min(nbin, noise_mask.get_nbin());

  if (verbose)
    cerr << "Pulsar::PolnProfileFit::get_variance noise mask uses "
	 << noise_mask.get_weight_sum() << " out of " << noise_mask.get_nbin()
	 << " harmonics" << endl;

  const float* amps = input->get_amps();

  unsigned count = 0;
  double total = 0;

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    if (ibin > mbin || noise_mask[ibin]) {
      float re = amps[ibin*2];
      float im = amps[ibin*2+1];
      total += re*re + im*im;
      count += 2;
    }


  // The variance of the spectrum (with zero mean) is the mean of the PSD
  return total / count;

}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::get_variance";
}

Matrix<4,4,double> 
Pulsar::PolnProfileFit::get_covariance (const PolnProfile* input) const try
{
  Matrix<4,4,double> covariance;

  unsigned npol = 4;

  for (unsigned ipol=0; ipol<npol; ipol++)
    for (unsigned jpol=ipol; jpol < npol; jpol++)
      covariance[ipol][jpol] = covariance[jpol][ipol] =
	get_covariance( input->get_Profile(ipol),
			input->get_Profile(jpol) ).real() / 2.0;

  //                                                      ^^^^^
  // divide by two because re and im are treated as unique measurements

  if (verbose)
    cerr << "Pulsar::PolnProfileFit::get_covariance\n" << covariance << endl;

  return covariance;
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::get_covariance";
}

complex<double> Pulsar::PolnProfileFit::get_covariance (const Profile* p1,
							const Profile* p2)
  const try
{
  unsigned nbin = p1->get_nbin()/2;
  unsigned mbin = std::min(nbin, noise_mask.get_nbin());

  if (verbose)
    cerr << "Pulsar::PolnProfileFit::get_variance noise mask uses "
	 << noise_mask.get_weight_sum() << " out of " << noise_mask.get_nbin()
	 << " harmonics" << endl;

  const complex<float>* amps1 = (complex<float>*) p1->get_amps();
  const complex<float>* amps2 = (complex<float>*) p2->get_amps();

  unsigned count = 0;
  complex<double> total = 0;

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    if (ibin > mbin || noise_mask[ibin]) {
      total += amps1[ibin] * conj(amps2[ibin]);
      count ++;
    }


  // The variance of the spectrum (with zero mean) is the mean of the PSD
  return total / complex<double>(count);
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileFit::get_covariance";
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
  FTransform::bcc1d (nbin, ccf, cpsd);

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
