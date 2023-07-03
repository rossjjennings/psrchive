/***************************************************************************
 *
 *   Copyright (C) 2002 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/ScalarTemplateMatching.h"
#include "Pulsar/Profile.h"
#include "FTransform.h"

// #define _DEBUG 1
#include "debug.h"

#include <iostream>
#include <assert.h>
#include <math.h>

using namespace std;

#define SQR(x) ((x)*(x))

#define F77_fccf F77_FUNC(fccf,FCCF)

extern "C" {
  void F77_fccf (float *, float *, float *);
}

double zbrent(float low_tau, float high_tau,
	              float low_deriv_chisq, float high_deriv_chisq, 
	              float edtau, int narrays, 
	              const vector< vector<float> >& xcorr_amps, 
		      const vector< vector<float> >& xcorr_phases, int nsum)
{
  int i_max = 100;
  double eps = .00000006;
  double a = low_tau;
  double b = high_tau;
  double c = high_tau; 
  double d = 0;
  double e = 0;
  double fa = low_deriv_chisq; 
  double fb = high_deriv_chisq;
  double fc = fb;


  for (int i=1; i<i_max; ++i) {
    if(fb*fc>0){
      c  = a;
      fc = fa;
      d  = b-a;
      e  = d;
    }
    if(fabs(fc)<fabs(fb)){
      a  = b;
      b  = c;
      c  = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }
    double tol1 = 2*eps *fabs(b)+.5*edtau;
    double xm = .5*(c-b);
    if(fabs(xm)<=tol1 || fb == 0) return(b);
    if(fabs(e)>=tol1 && fabs(fa)>fabs(fb)){
      double p, q, r;
      double s = fb/fa;
      if(a==c){
	p = 2*xm*s;
	q = 1-s;
      } else {
	q = fa/fc;
	r = fb/fc;
	p = s*(2*xm*q*(q-r)-(b-a)*(r-1));
	q = (q-1)*(r-1)*(s-1);
      }
      if(p>0) q = -q;
      p = fabs(p);
      if(2*p<(3*xm*q-fabs(tol1*q)) && 2*p<fabs(e*q)){
	e = d;
	d = p/q;
      } else {
	d = xm;
	e = d;
      }
    } else {
      d = xm;
      e = d;
    }
    a = b;
    fa = fb;
    if(fabs(d)>tol1) b+=d;
    else b+=tol1*xm/fabs(xm);
    fb = 0;
    for(int j=0; j<narrays; ++j) {
      for(int iter=1; iter<nsum; ++iter)
	fb+=iter*xcorr_amps[j][iter]*sin(-xcorr_phases[j][iter]+iter*b); }
  }
  return(b);
}

void Pulsar::ScalarTemplateMatching::model_profile ()
{
  unsigned narrays = data.size();

  if (narrays == 0)
    return;

  unsigned nbin_std = data[0].get_standard()->get_nbin();
  unsigned nbin_obs = data[0].get_observation()->get_nbin();
  unsigned npts = std::min(nbin_std, nbin_obs);
  unsigned npt2 = npts/2;

  DEBUG("ScalarTemplateMatching::model_profile nbin_std=" << nbin_std << " nbin_obs=" << nbin_obs << " npts=" << npts << " npt2=" << npt2 << " n_harmonic=" << n_harmonic);

  if (n_harmonic < npt2)
    npt2 = n_harmonic;

  // work around bug #390 - part 1
  // fccf assumes that there are at least 16 harmonics in the input
  const unsigned fccf_min_size = 32;
  unsigned xcorr_size = std::max (npt2, fccf_min_size);

  vector< vector<float> > xcorr_amps(narrays);
  vector< vector<float> > xcorr_phases(narrays);

  // Account for any offset between the centres of bin 0 of the standard (template) and observed profiles.
  double offset = M_PI * (1.0/nbin_obs - 1.0/nbin_std);

  for (unsigned i=0; i<narrays; ++i)
  {
    xcorr_amps[i].resize(xcorr_size);
    xcorr_phases[i].resize(xcorr_size);

    const float* fft_std = data[i].get_standard_fourier()->get_amps(); 
    const float* fft_prf = data[i].get_observation_fourier()->get_amps(); 

    // expect the imaginary part of DC to be zero for real-valued input profiles
    // cerr << "obs: " << fft_prf[1] << " std:" << fft_std[1] << endl;

    for (unsigned j=0; j<npt2; ++j)
    {
      xcorr_amps[i][j] = sqrt( SQR(fft_std[2*j]) + SQR(fft_std[2*j+1]) ) * sqrt( SQR(fft_prf[2*j]) + SQR(fft_prf[2*j+1]) );
      xcorr_phases[i][j] = offset*j + atan2(fft_std[2*j+1], fft_std[2*j]) - atan2(fft_prf[2*j+1], fft_prf[2*j]);
    }

    // work around bug #390 - part 2
    // if npt2 < 16, fill the remainder of the fccf input arrays with zeroes
    for (unsigned j=npt2; j<xcorr_size; ++j)
      xcorr_amps[i][j] = xcorr_phases[i][j] = 0.0;
  }

  // Compute an initial estimation of the shift based on the
  // (discrete) cross-correlation function

  float xcorr_shift;
  F77_fccf (&(xcorr_amps[0][1]), &(xcorr_phases[0][1]), &xcorr_shift);

  DEBUG("ScalarTemplateMatching::model_profile xcorr_shift=" << xcorr_shift);

  // Run through successively larger numbers of frequency components
  // finding the best-fitting shift between the two profiles. We
  // start at 32 frequency components and continue up until npts/2
  // components.
  //
  // N.B. we do not include the DC component of the fourier transform
  // in our sums.

  double tau = (double) xcorr_shift;
  double dtau = 0, edtau = 0;
  double deriv_chisq = 0;
  double low_tau = 0, low_deriv_chisq = 0, high_tau = 0, high_deriv_chisq = 0;
  int start_bin = fccf_min_size;

  // Allow this loop index to go all the way up to npts/2, the second
  // loop will ensure the that the old Nyquist index is not accessed

  for (unsigned nsum=start_bin; nsum<=npt2; nsum*=2)
  {
    dtau = 2*M_PI/(float)(5.0*nsum);
    edtau = 1.0/(float)(2.0*nsum+1.0);
    if(nsum>npts/4.0) edtau = .00001;

    int ntries = 0;
    int low = 0, high = 0;

    while (low == 0 || high == 0)
    {
      ntries++;
      if (ntries > 100)
      {
	      throw Error (InvalidState, "ScalarTemplateMatching::model_profile",
                      "max retries (100) exceeded");
      }

      deriv_chisq = 0;
      for(unsigned i=0; i<narrays; ++i)
	      for(unsigned j=1; j<nsum; ++j)
	        deriv_chisq+=j*xcorr_amps[i][j]*sin(-xcorr_phases[i][j]+j*tau);

      if (deriv_chisq<0)
      {
        low_tau = tau;
        low_deriv_chisq = deriv_chisq;
        tau += dtau;
        low = 1;
      }
      else
      {
        high_tau = tau;
        high_deriv_chisq = deriv_chisq;
        tau -= dtau;
        high = 1;
      }
    }

    tau = zbrent (low_tau, high_tau, low_deriv_chisq, high_deriv_chisq, 
		  edtau, narrays, xcorr_amps, xcorr_phases, nsum);
  }

  if (verbose) 
    cerr << "ScalarTemplateMatching::model_profile best tau is " << tau << endl;

  // These relationships are discussed in Joe Taylor's paper, "Pulsar
  // Timing and Relativistic Gravity", Philosophical Transactions:
  // Physical Sciences and Engineering, Vol. 341, No. 1660, "Pulsars
  // as Physics Laboratories" (Oct 15, 1992), pages 117-134.

  // The profile is related to the standard by:
  //
  // P(t) = a + scale*S(t-tau) + N(t)
  //
  // where:
  //   P = test profile 
  //   S = standard template
  //   N = noise 
  // tau = shift between profile and template

  double s1=0, s2=0, s3=0;
  double cosfac;
  for(unsigned i=0; i<narrays; ++i)
  {
    const float* fft_std = data[i].get_standard_fourier()->get_amps(); 
    for(unsigned j=1; j<npt2; ++j)
    {
      cosfac = cos(-xcorr_phases[i][j]+j*tau);
      s1 += xcorr_amps[i][j]*cosfac;
      s2 += SQR(fft_std[2*j]) + SQR(fft_std[2*j+1]);
      s3 += j*j*xcorr_amps[i][j]*cosfac;
    }
  }

  if (s1<=0 || s2==0 || s3<=0)
  {
    if (s1<=0)
      cerr << "  Numerator to Equation A9 = " << s1 << endl;
    if (s2==0)
      cerr << "  Denominator to Equations A9 and A11 equals zero" << endl;
    if (s3<=0)
      cerr << "  Denominator to Equation A10 = " << s3 << endl;

	  throw Error (InvalidState, "ScalarTemplateMatching::model_profile",
                "aborting before floating exception");
  }

  double scale = s1/s2;
  DEBUG("ScalarTemplateMatching::model_profile scale=" << scale);

  chisq = 0;
  for(unsigned i=0; i<narrays; ++i)
  {
    const float* fft_std = data[i].get_standard_fourier()->get_amps(); 
    const float* fft_prf = data[i].get_observation_fourier()->get_amps(); 

    for(unsigned j=1; j<npt2; ++j)
    {
      const double fft_prf_amp = sqrt( SQR(fft_prf[2*j]) + SQR(fft_prf[2*j+1]) );
      const double fft_std_amp = sqrt( SQR(fft_std[2*j]) + SQR(fft_std[2*j+1]) );
      chisq += fft_prf_amp*fft_prf_amp - 2*scale*fft_prf_amp*fft_std_amp*cos(xcorr_phases[i][j]-j*tau) + scale*scale*fft_std_amp*fft_std_amp;
    }
  }

  if (chisq <= 0)
  {
	  throw Error (InvalidState, "ScalarTemplateMatching::model_profile",
                "model_profile: chisq <= 0");
  }

  // Subtract 1 from npt2 because DC is skipped; Subtract 1 from total D.O.F. for tau.
  nfree = narrays*(npt2-1) - 1;

  /* 
   * 2023-Jun-29 - Willem van Straten
   *
   * To date, the following line has been the default behaviour of this algorithm.
   * By setting the variance equal to the reduced chisq (computed with variance equal to 1), 
   * the reduced chisq is set to one.
   * 
   * See the discussion in the appendix of Rogers et al (2023).
   */

  // This defines the errors in scale and shift so that the reduced chisq is unity.
  double var = chisq / nfree;

  if (compute_reduced_chisq)
  {
    /*
     * 2023-Jun-29 - Willem van Straten
     *
     * Setting compute_reduced_chisq == true enables correct calculation of
     * the reduced chisq using expected variance of the residual profile.
     */

    // note that this is valid only when data.size == 1 (single template and profile)
    var = data[0].get_residual_variance (scale);
    chisq /= var;
    DEBUG("ScalarTemplateMatching::model_profile reduced chisq=" << chisq / nfree);
  }
  else
  {
    // make the reduced chisq = 1 assumption explicit and visible
    chisq = nfree;
  }

  // best-fit scale
  best_scale.val = scale;
  best_scale.var = var / (2.0*s2);

  // best-fit shift in radians
  best_shift.val = tau;
  best_shift.var = var / (2.0* scale * s3);
}