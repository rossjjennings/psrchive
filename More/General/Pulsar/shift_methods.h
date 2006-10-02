//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_shift_methods_h
#define __Pulsar_shift_methods_h

#include "Estimate.h"

namespace Pulsar {

  class Profile;

  /*! Parabolic interpolation in the time domain */
  Estimate<double> ParIntShift (const Profile& std, const Profile& ephase);

  /*! Gaussian interpolation in the time domain */
  Estimate<double> GaussianShift (const Profile& std, const Profile& ephase);

  /*! Fourier domain zero-pad interpolation */
  Estimate<double> ZeroPadShift (const Profile& std, const Profile& ephase);

  /*! Fourier domain phase gradient fit */
  Estimate<double> PhaseGradShift (const Profile& std, const Profile& ephase); 

  /*! Phase shift by sinc interpolation of CCF */
  Estimate<double> SincInterpShift (const Profile& std, const Profile& ephase);

}

#endif

