/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AdaptiveSmooth.h"
#include "Pulsar/Profile.h"
#include "FTransform.h"

#include <complex>
#include <math.h>

using namespace std;

Pulsar::AdaptiveSmooth::AdaptiveSmooth ()
{
  max_harm=0;
  hold=false;
  noise_band=0.3;
  method = Sinc;
}

Pulsar::AdaptiveSmooth::~AdaptiveSmooth ()
{

}

float Pulsar::AdaptiveSmooth::get_turns () const
{
  double sum=0.0;
  for (unsigned i=0; i<filter.size(); i++) { sum += filter[i]; }
  return 1.0/sum;
}

unsigned Pulsar::AdaptiveSmooth::get_max_harm () const
{
  return max_harm;
}

void Pulsar::AdaptiveSmooth::set_hold(bool h) { hold=h; }

bool Pulsar::AdaptiveSmooth::get_hold() const { return hold; }

void Pulsar::AdaptiveSmooth::set_method(Pulsar::AdaptiveSmooth::Method m) {
  method = m;
}

void Pulsar::AdaptiveSmooth::transform(Profile *p)
{

  // Make fft of profile
  float *fprof = new float[p->get_nbin() + 2];
  FTransform::frc1d(p->get_nbin(), fprof, p->get_amps());

  // If no current filter or hold is off, compute optimal filter
  if (filter.size()==0 || !hold) 
    compute(fprof, p->get_nbin());

  // Apply filter to profile
  unsigned ih;
  const int nh = p->get_nbin()/2 + 1;
  complex<float> *cprof = (complex<float> *)fprof;
  for (ih=0; (ih<nh) && (ih<filter.size()); ih++) 
    cprof[ih] *= filter[ih];
  for (ih=ih; ih<nh; ih++)
    cprof[ih] = 0.0;

  // Inverse transform back to profile
  FTransform::bcr1d(p->get_nbin(), p->get_amps(), fprof);
  if (FTransform::get_norm() == FTransform::unnormalized)
    p->scale(1.0 / (double)p->get_nbin());

  // Free temp space
  delete [] fprof;
}

void Pulsar::AdaptiveSmooth::compute(const float *fprof, int nbin)
{
  // Number of harmonics (including DC)
  const int nh = nbin/2 + 1;

  // Reset existing filter
  filter.resize(nh);
  for (unsigned i=0; i<nh; i++) 
    filter[i] = 0.0;

  // Compute profile power spectrum
  float *pspec = new float[nh];
  const complex<float> *cprof = (const complex<float> *)fprof;
  for (unsigned i=0; i<nh; i++) 
    pspec[i] = norm(cprof[i]);

  // Estimate noise level
  double sigma2=0.0;
  int count=0;
  for (unsigned i=(1.0 - noise_band)*nh; i<nh; i++) {
    sigma2 += pspec[i];
    count++;
  }
  sigma2 /= (double)count;

  // Use a specific implementation
  if (method==Wiener)
    compute_wiener(pspec, sigma2, nh);
  else if (method==Sinc)
    compute_lpf_sinc(pspec, sigma2, nh);
  else 
    throw Error(InvalidState, "Pulsar::AdaptiveSmooth::compute",
        "Invalid filter method selected");

}

void Pulsar::AdaptiveSmooth::compute_wiener(const float *pspec,
    double sigma2, int nh) {
  // Basic Wiener filter (ie, see NR sec 13.3), with a 2-sigma
  // cutoff to remove noisy points.
  filter[0] = 1.0;
  for (unsigned i=1; i<nh; i++) {
    filter[i] = (pspec[i] - sigma2) / pspec[i];
    if (filter[i]<0.5) filter[i]=0.0;
  }
}

void Pulsar::AdaptiveSmooth::compute_lpf_sinc(const float *pspec,
    double sigma2, int nh) 
{
  // Just brute-force this minimization:
  // Criterion is minimum expected MS diff between real and noisy signal,
  // same as in Wiener filter.  But here our denoising filter is 
  // constrained to be a "brick wall" (sinc in time domain). We ignore
  // DC in making this.
  double mse, mse_min;
  int ih_min;

  // Get total signal power
  double ptot=0.0;
  for (unsigned i=1; i<nh; i++) 
    ptot += pspec[i];

  // Init with values for first harm
  ih_min = 1;
  ptot -= pspec[1];
  mse_min = sigma2 + ptot - (double)(nh-2)*sigma2;

  // Loop over harmonics, look for min
  for (unsigned i=2; i<nh; i++) {
    ptot -= pspec[i];
    mse = (double)i*sigma2 + ptot - (double)(nh-1-i)*sigma2;
    if (mse < mse_min) { 
      mse_min = mse;
      ih_min = i;
    }
  }

  // Fill in filter coeffs
  for (unsigned i=0; i<=ih_min; i++) 
    filter[i] = 1.0;
  for (unsigned i=ih_min+1; i<nh; i++) 
    filter[i] = 0.0;

  // fill in etc
  max_harm = ih_min;
}

void Pulsar::AdaptiveSmooth::compute_lpf(const float *pspec,
    double sigma2, int nh, float (*filter_func)(float,float))
{
  // Same minimization as above, but uses a generic filter
  // shape that takes a freq and cutoff freq as 
  // arguments.
  double mse, mse_min=0.0;
  int ih_min=0;

  for (unsigned ih=1; ih<nh*(1.0-noise_band); ih++) {

    // Compute MSE using ih as cutoff freq.
    mse=0.0;
    for (unsigned i=1; i<nh; i++) {
      double g = filter_func((float)i, (float)ih);
      mse += ((1.0-g)*(1.0-g)*(pspec[i]-sigma2) + g*g*sigma2);
    }

    // Keep this one if mse is minimum
    if (ih==1) {
      ih_min = 1;
      mse_min = mse;
    } else {
      if (mse<mse_min) {
        ih_min = ih;
        mse_min = mse;
      }
    }
  }

  // Fill in filter coeffs
  for (unsigned ih=0; ih<nh; ih++) 
    filter[ih] = filter_func((float)ih, (float)ih_min);

}
