//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/ProfileStandardShift.h

#ifndef __Pulsar_ProfileStandardShift_h
#define __Pulsar_ProfileStandardShift_h

#include "Pulsar/ProfileShiftEstimator.h"

namespace Pulsar {

  //! Estimates the phase shift relative to a standard pulse profile
  class ProfileStandardShift : public ProfileShiftEstimator
  {

  public:

    //! Set the profile with respect to which the shift will be estimated
    virtual void set_standard (const Profile* p) { standard = p; }

    //! Get the profile with respect to which the shift will be estimated
    const Profile* get_standard () const { return standard; }

  protected:

    Reference::To<const Profile> standard;
  };

}


#endif // !defined __Pulsar_ProfileStandardShift_h
