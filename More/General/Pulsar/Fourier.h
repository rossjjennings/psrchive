//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Fourier.h,v $
   $Revision: 1.3 $
   $Date: 2007/10/02 05:19:48 $
   $Author: straten $ */

#ifndef __Pulsar_Fourier_h
#define __Pulsar_Fourier_h

#include "FTransformAgent.h"

namespace Pulsar {

  class Profile;
  class PolnProfile;

  //! Return the forward Fourier transform of the input Profile
  Profile* fourier_transform (const Profile* input);

  //! Return the forward Fourier transform of the input PolnProfile
  PolnProfile* fourier_transform (const PolnProfile* input);

  //! Square-law detect the input Profile
  void detect (Profile* input);

  //! Square-law detect the input PolnProfile
  void detect (PolnProfile* input);

  //! If set, fourier_transform functions use this plan without question
  extern FTransform::Plan* fourier_transform_plan;

}

#endif
