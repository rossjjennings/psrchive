//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/model_profile.h,v $
   $Revision: 1.1 $
   $Date: 2002/04/19 02:00:32 $
   $Author: straten $ */

#ifndef __model_profile_h
#define __model_profile_h


double zbrent (float low_tau, float high_tau, float low_chisq,
               float high_chisq, float edtau, int narrays,
               float ** xcorr_amps, float ** xcorr_phases, int nsum);

int model_profile (int nbin, int narrays, float ** prf, float ** std,
                   double * scale, double * sigma_scale,
                   double * shift, double * sigma_shift, double * chisq,
                   int verbose);


#endif
