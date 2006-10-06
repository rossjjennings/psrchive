//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Fourier.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_Fourier_h
#define __Pulsar_Fourier_h

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

}

#endif
