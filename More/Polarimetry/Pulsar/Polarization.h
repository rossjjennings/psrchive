//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Polarization.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/09 21:09:35 $
   $Author: straten $ */

#ifndef __Pulsar_polarization_h
#define __Pulsar_polarization_h

// this file defines some convenience routines for use in polarization work

namespace Pulsar {

  class Archive;
  class PolnProfile;
  class Profile;

  //! Return a newly constructed PolnProfile with state == Stokes
  const PolnProfile* new_Stokes (const Archive*, unsigned sub, unsigned chan);

  //! Return a possibly newly constructed Profile with state specified by code
  const Profile* new_Profile (const PolnProfile* data, char code);

}


#endif



