//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/model_profile.h,v $
   $Revision: 1.3 $
   $Date: 2004/05/02 08:34:03 $
   $Author: sord $ */

#ifndef __model_profile_h
#define __model_profile_h

namespace Pulsar {

  double zbrent (float low_tau, float high_tau, float low_chisq,
                 float high_chisq, float edtau, int narrays,
                 float ** xcorr_amps, float ** xcorr_phases, int nsum);
  
  int model_profile (int nbin, int narrays, float ** prf, float ** std,
                     double * scale, double * sigma_scale,
                     double * shift, double * sigma_shift, double * chisq,
                     int verbose);

  int legacy_fftconv (int nbin, float *_prof, float *_std, double * _shift, 
                      double *_eshift, float *_snrfft, float *_esnrfft);;

}

#endif
