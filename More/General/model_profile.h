//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/model_profile.h

#ifndef __model_profile_h
#define __model_profile_h

namespace Pulsar {

  double zbrent (float low_tau, float high_tau, float low_chisq,
                 float high_chisq, float edtau, int narrays,
                 float ** xcorr_amps, float ** xcorr_phases, int nsum);
  
  int model_profile (int nbin, int narrays,
                     const float * const * prf, const float * const * std,
                     double * scale, double * sigma_scale,
                     double * shift, double * sigma_shift, double * chisq,
                     int verbose);

  extern int max_harmonic;

}

#endif
