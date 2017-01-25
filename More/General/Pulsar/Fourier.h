//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Fourier.h

#ifndef __Pulsar_Fourier_h
#define __Pulsar_Fourier_h

#include "FTransformAgent.h"

namespace Pulsar {

  class Profile;
  class PolnProfile;

  //! Return the forward Fourier transform of the input Profile
  Profile* fourier_transform (const Profile*,
			      FTransform::Plan* = 0);

  //! Return the forward Fourier transform of the input PolnProfile
  PolnProfile* fourier_transform (const PolnProfile*,
				  FTransform::Plan* = 0);

  //! Return the forward Fourier transform of the input Profile
  Profile* complex_fourier_transform (const Profile*,
				      FTransform::Plan* = 0);

  //! Return the forward Fourier transform of the input PolnProfile
  PolnProfile* complex_fourier_transform (const PolnProfile*,
					  FTransform::Plan* = 0);

  //! Square-law detect the input complex-valued Profile
  void detect (Profile* input);

  //! Compute the phase of the input complex-valued Profile
  void phase (Profile* input);

  //! Square-law detect the input complex-valued PolnProfile
  void detect (PolnProfile* input);

  //! Compute the phase of the input complex-valued PolnProfile
  void phase (PolnProfile* input);

}

#endif
