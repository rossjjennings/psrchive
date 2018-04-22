/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/ProfileShiftFit.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/Profile.h"

#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/LastHarmonic.h"
#include "Pulsar/Fourier.h"

#include "toa.h"

#include "Warning.h"
#include "FTransform.h"
#include "Brent.h"
#include "BoxMuller.h"
#include <complex>
#include <stdlib.h>
#include <string.h>

#if HAVE_GSL
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#endif

using namespace std;

static Warning warn;

void Pulsar::ProfileShiftFit::init ()
{
  nharm=0;

  std=NULL;
  fstd=NULL;
  prof=NULL;
  fprof=NULL;
  fccf=NULL;

  std_pow=0.0;

  nbins_prof=0;
  nbins_std=0;
  nbins_ccf=0;

  err_meth = Traditional_Chi2;

  mcmc_it = 1000;

  computed=false;
  shift=0.0;
  eshift=0.0;
  scale=0.0;
  escale=0.0;
  sigma2=0.0;
  mse=0.0;

#if HAVE_GSL
  gsl_set_error_handler_off();
#endif

}

Pulsar::ProfileShiftFit::ProfileShiftFit()
{
  init();
  choose_maximum_harmonic = false;
}

Pulsar::ProfileShiftFit::~ProfileShiftFit() { reset(); }

void Pulsar::ProfileShiftFit::reset()
{
  if (fstd!=NULL) delete [] fstd;
  if (fprof!=NULL) delete [] fprof;
  if (fccf!=NULL) delete [] fccf;
  init();
}

void Pulsar::ProfileShiftFit::set_nharm(unsigned nh) 
{ 
  // Save value
  nharm = nh; 

  // Allocate space for ccf
  unsigned tmp = 2*(nh + 1);
  nbins_ccf = 1;
  while (nbins_ccf<tmp) nbins_ccf <<= 1;
  if (fccf!=NULL) delete[] fccf;
  fccf = new float[nbins_ccf + 2];

  // If std set, compute normalization
  if (fstd!=NULL)
  {
    std_pow = 0.0;
    complex<float> *cstd  = (complex<float> *)fstd;
    for (unsigned ih=1; ih<=nharm; ih++) 
      std_pow += norm(cstd[ih]);
  }

  if (nbins_prof)
    effective_nharm = std::min(nharm, nbins_prof/2 - 1);

  // Reset valid flag
  computed = false;
}

unsigned Pulsar::ProfileShiftFit::get_nharm()
{
  return(nharm);
}

void Pulsar::ProfileShiftFit::set_standard (const Profile *p)
{
  // Reset when changing template to avoid inconsistency
  reset();

  // Save a pointer to the template
  std = p;
  nbins_std = std->get_nbin();

  // Alloc memory, FFT
  if (fstd!=NULL) delete [] fstd;
  fstd = new float[std->get_nbin() + 2];
  FTransform::frc1d(std->get_nbin(), fstd, std->get_amps());

  double fnorm = 1.0/FTransform::get_scale (std->get_nbin(), FTransform::frc);
  for (unsigned i=0; i<std->get_nbin() + 2; i++)
    fstd[i] *= fnorm*fnorm;

  // Set up nharm, default to use whole template
  if (choose_maximum_harmonic)
    choose_nharm ();
  else if (nharm==0 || nharm > std->get_nbin()/2 - 1) 
    set_nharm(std->get_nbin() / 2 - 1);
  else 
    set_nharm(nharm);

  // Reset valid flag
  computed = false;
}

void Pulsar::ProfileShiftFit::choose_nharm ()
{
  Profile temp_std_psd (std->get_nbin());
  temp_std_psd.set_amps (fstd);
  detect (&temp_std_psd);

  LastHarmonic last;
  last.set_Profile( &temp_std_psd );
  set_nharm ( last.get_last_harmonic() );
}

void Pulsar::ProfileShiftFit::set_Profile (const Profile *p)
{
  // Need to set template first
  if (fstd==NULL)
    throw Error (InvalidState, "Pulsar::ProfileShiftFit::set_Profile",
        "set_standard() must be called before set_Profile()");

  // Save profile pointer
  prof = p;

  // Alloc new mem if we need to
  if (prof->get_nbin() != nbins_prof)
  {
    nbins_prof = prof->get_nbin();
    if (fprof!=NULL) delete [] fprof;
    fprof = new float[nbins_prof + 2];
  }

  effective_nharm = std::min(nharm, nbins_prof/2 - 1);

  // FFT profile
  FTransform::frc1d(nbins_prof, fprof, prof->get_amps());

  double fnorm = 1.0/FTransform::get_scale (nbins_prof, FTransform::frc);
  for (unsigned i=0; i<nbins_prof + 2; i++)
    fprof[i] *= fnorm*fnorm;

  // Calc CCF
  for (unsigned i=0; i<nbins_ccf+2; i++) fccf[i]=0.0;
  complex<float> *cstd  = (complex<float> *)fstd;
  complex<float> *cprof = (complex<float> *)fprof;
  complex<float> *cccf  = (complex<float> *)fccf;

  for (unsigned ih=1; ih<=effective_nharm; ih++) 
    cccf[ih] = conj(cstd[ih]) * cprof[ih];

  // Reset valid flag
  computed = false;
}

double Pulsar::ProfileShiftFit::ccf(double phi) 
{
  if (fccf==NULL || fprof==NULL || fstd==NULL) 
    throw Error (InvalidState, "Pulsar::ProfileShiftFit::ccf",
        "ccf() called before standard and data set");

  double result=0.0, arg;
  complex<float> *cccf  = (complex<float> *)fccf;
  complex<double> e;

  for (unsigned ih=1; ih<=effective_nharm; ih++) {
    arg = 2.0*M_PI*phi*(double)ih;
    result += (complex<double>(cccf[ih]) 
        * exp(complex<double>(0.0,arg))).real();
  }
  return(result);
}

double Pulsar::ProfileShiftFit::dccf(double phi)
{
  if (fccf==NULL || fprof==NULL || fstd==NULL) 
    throw Error (InvalidState, "Pulsar::ProfileShiftFit::dccf",
        "dccf() called before standard and data set");

  double result=0.0, tmp;
  complex<float> *cccf  = (complex<float> *)fccf;
  complex<double> e;

  for (unsigned ih=1; ih<=effective_nharm; ih++) {
    tmp = 2.0*M_PI*(double)ih;
    result += (complex<double>(cccf[ih]) 
        * complex<double>(0.0,tmp)
        * exp(complex<double>(0.0,tmp*phi))).real();
  }
  return(result);
}

double Pulsar::ProfileShiftFit::d2ccf(double phi)
{
  if (fccf==NULL || fprof==NULL || fstd==NULL) 
    throw Error (InvalidState, "Pulsar::ProfileShiftFit::d2ccf",
        "d2ccf() called before standard and data set");

  double result=0.0, tmp;
  complex<float> *cccf  = (complex<float> *)fccf;
  complex<double> e;

  for (unsigned ih=1; ih<=effective_nharm; ih++) {
    tmp = 2.0*M_PI*(double)ih;
    result += (complex<double>(cccf[ih]) 
        * (-tmp * tmp)
        * exp(complex<double>(0.0,tmp*phi))).real();
  }
  return(result);
}

// The actual timing fit
void Pulsar::ProfileShiftFit::compute()
{

  // Get a rough estimate by finding time domain CCF max.
  float *tccf = new float[nbins_ccf+2];
  float *fccf_tmp = new float[nbins_ccf+2];
  memcpy(fccf_tmp, fccf, sizeof(float)*(nbins_ccf+2)); // c2r destroys input
  FTransform::bcr1d(nbins_ccf, tccf, fccf_tmp);
  delete [] fccf_tmp;
  int max_bin=0;
  float max_ccf = tccf[0];
  for (unsigned i=1; i<nbins_ccf; i++) {
    if (tccf[i]>max_ccf) { max_ccf=tccf[i]; max_bin=i; }
  }
  double phi, phi_lo, phi_hi;
  phi  = (double)max_bin/(double)nbins_ccf;
  phi_lo = ((double)max_bin-1.0)/(double)nbins_ccf;
  phi_hi = ((double)max_bin+1.0)/(double)nbins_ccf;

  // Refine using Brent's method
  Functor<double(double)> f (this, &Pulsar::ProfileShiftFit::ccf);
  try { phi = Brent_min(f, phi_lo, phi, phi_hi, 1e-9, -1); }
  catch (Error &e) {
    // If that didn't converge, we probably have noisy data
    // that won't get sub-bin precision anyways, so just stick
    // with the original estimate for now.
    warn << "Brent_min didn't converge" << endl;
    phi = (double)max_bin / (double)nbins_ccf;
  }
  max_ccf = ccf(phi);

  // Calc data power
  double prof_pow=0.0;
  complex<float> *cprof = (complex<float> *)fprof;

  for (unsigned ih=1; ih<=effective_nharm; ih++) 
    prof_pow += norm(cprof[ih]);

  // Calc sigma2 from upper freq part of data
  // TODO:
  //   1. Start at 1/2 or 3/4 harmonic?
  //   2. Use median instead of mean?
  sigma2 = 0.0;
  int count=0;
  for (unsigned ih=effective_nharm+1; ih<=nbins_prof/2; ih++) {
    count++;
    sigma2 += norm(cprof[ih]);
  }
  sigma2 /= 2.0*(double)count;

  // Put params/etc in correct place

  // Correct shift for different FFT length phase offset, if any
  correction = 0.5/nbins_prof - 0.5/nbins_std;
  //shift = phi + correction;
  shift = phi;

  scale = max_ccf / std_pow;
  dof = 2*effective_nharm - 2;
  chi2 = (prof_pow - scale*max_ccf)/sigma2;
  mse = (prof_pow - scale*max_ccf)/(double)dof;

  snr = (scale*scale*std_pow - sigma2) / mse;
  snr = (snr<0.0) ? 0.0 : sqrt(snr);

  // Estimate param errors
  switch (err_meth) {
    case MCMC_Variance:
      error_mcmc_pdf_var();
      break;
    case Numerical:
      error_numerical();
      break;
    case Traditional_Chi2:
    default:
      error_traditional();
  }

  // Set valid flag
  computed = true;
}

double Pulsar::ProfileShiftFit::log_shift_pdf(double phi)
{
  double c = ccf(phi);
  c = (c*c)/(2.0*mse*std_pow);
  return(c);
}

double Pulsar::ProfileShiftFit::log_shift_pdf_pos(double phi)
{
  double c = ccf(phi);
  c /= sqrt(2.0*mse*std_pow);
  return(c*c + log(1.0 + erf(c)));
}

void Pulsar::ProfileShiftFit::error_traditional()
{
  // "Traditional" uncertainty calc based on Chi^2 2nd derivs
  eshift = sqrt(mse / (-1.0 * scale * d2ccf(shift)));
  escale = sqrt(mse / std_pow);
}

#if HAVE_GSL

// Wrapper for GSL integators
double Pulsar::ProfileShiftFit::f_pdf(double phi, void *_psf)
{
  Pulsar::ProfileShiftFit *psf = (Pulsar::ProfileShiftFit *)_psf;
  double ll = psf->log_shift_pdf_pos(phi) - psf->max_log_pdf;
  return exp(ll);
}

// Wrapper for GSL integators
double Pulsar::ProfileShiftFit::f_pdf_x2(double phi, void *_psf)
{
  Pulsar::ProfileShiftFit *psf = (Pulsar::ProfileShiftFit *)_psf;
  double ll = psf->log_shift_pdf_pos(phi) - psf->max_log_pdf;
  return exp(ll + 2.0*log(abs(phi-psf->shift)));
}

static void integration_error_check(int rv)
{
  if (rv==0) return;
  if (rv==GSL_EROUND)
    warn << "ProfileShiftFit: roundoff error in uncertainty integration" << endl;
  else 
    throw Error (FailedCall, "Pulsar::ProfileShiftFit::do_integration",
        "GSL error: '%s'", gsl_strerror(rv));
}

static double do_integration(gsl_function *func, double shift, double eshift,
    gsl_integration_workspace *w)
{
  int rv;
  double ftmp, etmp, result=0.0;
  const double ftol = 1e-4;
  double cutoff = 5.0*eshift;

  if (cutoff > 0.5) { cutoff = 0.25; }

  rv = gsl_integration_qag(func, 
      shift-0.5, 
      shift-cutoff,
      0.0, ftol, 
      w->limit, GSL_INTEG_GAUSS15, w,
      &ftmp, &etmp);
  result += ftmp;
  //cerr << "left=" << ftmp;
  integration_error_check(rv);

  rv = gsl_integration_qag(func, 
      shift-cutoff,
      shift+cutoff,
      0.0, ftol, 
      w->limit, GSL_INTEG_GAUSS21, w,
      &ftmp, &etmp);
  result += ftmp;
  //cerr << " middle=" << ftmp;
  integration_error_check(rv);

  rv = gsl_integration_qag(func, 
      shift+cutoff,
      shift+0.5,
      0.0, ftol, 
      w->limit, GSL_INTEG_GAUSS15, w,
      &ftmp, &etmp);
  result += ftmp;
  //cerr << " right=" << ftmp << endl;
  integration_error_check(rv);

  return result;
}

void Pulsar::ProfileShiftFit::error_numerical()
{
  // Numerically integrate the PDF using GSL routines to get 
  // the uncertainty.

  // Init with usual errors
  error_traditional();

  // Use to rescale PDF; should make area~1.0 if normal approx is valid
  max_log_pdf = log_shift_pdf_pos(shift) + 0.5*log(2.0*M_PI*eshift*eshift);
  
  // Set up workspace
  gsl_integration_workspace *w = gsl_integration_workspace_alloc(2*nbins_ccf); // ?

  //cerr << "nb=" << nbins_ccf << " w->size=" << w->size << " w->limit=" << w->limit << endl;

  double top=0.0, bot=0.0, etop, ebot;

  // Integrate PDF
  gsl_function F;
  F.function = &Pulsar::ProfileShiftFit::f_pdf;
  F.params = this;
  bot = do_integration(&F, shift, eshift, w);
  //cerr << "bot=" << bot << endl;

  // Integrate PDF*x^2
  F.function = &f_pdf_x2;
  top = do_integration(&F, shift, eshift, w);
  //cerr << "top=" << top << endl;

  //cerr << "II " << top << " " << bot << endl;

  eshift = sqrt(top/bot);

  gsl_integration_workspace_free(w);
}

#else // no GSL

void Pulsar::ProfileShiftFit::error_numerical()
{
  throw Error (InvalidParam, "Pulsar::ProfileShiftFit::error_numerical",
      "Numerical PDF integration requires GSL");
}

#endif

void Pulsar::ProfileShiftFit::error_mcmc_pdf_var() 
{
  // First calculate usual errors
  error_traditional();

  // Init MCMC
  mcmc_init();

  // Run for some number of samples, calc variance relative to
  // best-fit position.  We'll try w/ no "burn-in" since we're
  // starting from a known high probability state.
  double sum=0.0; 
  for (int i=0; i<mcmc_it; i++) {
    // "Standard" variance
    double x = mcmc_sample() - shift;
    x -= trunc(x);
    x = fabs(x);
    if (x>0.5) x = 1.0 - x;
    sum += x*x;
  }
  sum /= (double)mcmc_it;
  eshift = sqrt(sum);
}

void Pulsar::ProfileShiftFit::mcmc_init()
{
  /* Init MCMC using current shift value */
  mcmc_state = shift;
  mcmc_log_pdf = log_shift_pdf_pos(shift);
  mcmc_trials=0;
  mcmc_accept=0;
}

double Pulsar::ProfileShiftFit::mcmc_sample()
{
  /* Computes a trial sample by stepping away from current state
   * using a normally distributed step size.  Sample is then either
   * accepted or rejected using the usual MCMC rules.  Updated
   * state is returned and stored in mcmc_state.
   * 
   * Could try independent rather than random walk sampling?
   */
  double trial_state = mcmc_state + nrand() * eshift * 2.0;
  if (fabs(trial_state)>1.0) trial_state = fmod(trial_state,1.0);
  if (trial_state<1.0) trial_state += 1.0;
  double trial_log_pdf = log_shift_pdf_pos(trial_state);
  double log_pdf_ratio = trial_log_pdf - mcmc_log_pdf;
  mcmc_trials++;
  if (log_pdf_ratio>=0.0) { 
    /* Accept the new sample */
    mcmc_state = trial_state;
    mcmc_log_pdf = trial_log_pdf;
    mcmc_accept++;
  } else {
    /* Accept with probability pdf_ratio */
    double rv = ((double)rand()) / RAND_MAX;
    if (log(rv) < log_pdf_ratio) {
      mcmc_state = trial_state;
      mcmc_log_pdf = trial_log_pdf;
      mcmc_accept++;
    }
  }
  return(mcmc_state);
}

Tempo::toa Pulsar::ProfileShiftFit::toa(const Integration* subint)
{
  if (!computed) compute();
  Tempo::toa result(Tempo::toa::Parkes);

  double per = subint->get_folding_period();
  result.set_frequency(prof->get_centre_frequency());
  result.set_arrival(subint->get_epoch() + per * shift);
  result.set_error(eshift * per * 1e6);
  result.set_telescope(subint->expert()->get_parent()->get_telescope());

  return(result);
}

Estimate<double> Pulsar::ProfileShiftFit::get_shift()
{
  if (!computed) compute();
  return(Estimate<double>(shift + correction, eshift*eshift));
}

Estimate<double> Pulsar::ProfileShiftFit::get_scale()
{
  if (!computed) compute();
  return(Estimate<double>(scale, escale*escale));
}

double Pulsar::ProfileShiftFit::get_mse()
{
  if (!computed) compute();
  return(mse);
}

double Pulsar::ProfileShiftFit::get_snr()
{
  if (!computed) compute();
  return(snr);
}

double Pulsar::ProfileShiftFit::get_sigma2()
{
  if (!computed) compute();
  return(sigma2);
}

double Pulsar::ProfileShiftFit::get_effective_duty_cycle () const
{
  if (!std)
    throw Error (InvalidState, "Pulsar::ProfileShiftFit::effective_duty_cycle",
		 "standard not set");

  ProfileStats stats;
  stats.set_profile (std);
  double total_flux = stats.get_total().get_value();
  double avg_flux = total_flux / stats.get_onpulse_nbin();

  double weighted_power = 0.0;
  complex<float>* cstd  = reinterpret_cast< complex<float>* > (fstd);

  for (unsigned ih=1; ih<=nharm; ih++) 
    weighted_power += ih*ih *norm(cstd[ih]);

  return avg_flux / (2*M_PI*sqrt(weighted_power));
}

//! Get the reduced chi-squared
double Pulsar::ProfileShiftFit::get_reduced_chisq () const
{
  if (!computed)
    const_cast<ProfileShiftFit*>(this)->compute();

  return chi2/dof;
}

//! Convenience function to determine, then apply, scale and shift
void Pulsar::ProfileShiftFit::apply_scale_and_shift(Profile *p)
{
  set_Profile(p);
  compute();
  p->rotate_phase(shift);
  p->scale(1.0/scale);
}

