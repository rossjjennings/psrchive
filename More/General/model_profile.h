//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/model_profile.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/17 13:34:40 $
   $Author: straten $ */

#ifndef __model_profile_h
#define __model_profile_h

namespace Pulsar {

  double zbrent (float low_tau, float high_tau, float low_chisq,
                 float high_chisq, float edtau, int narrays,
                 float ** xcorr_amps, float ** xcorr_phases, int nsum);
  
  int model_profile (int nbin, int narrays,
                     float * const * prf, float * const * std,
                     double * scale, double * sigma_scale,
                     double * shift, double * sigma_shift, double * chisq,
                     int verbose);

  int legacy_fftconv (int nbin, const float *_prof, const float *_std, 
                      double * _shift, double *_eshift,
                      float *_snrfft, float *_esnrfft);

  extern int max_harmonic;
}

#endif
