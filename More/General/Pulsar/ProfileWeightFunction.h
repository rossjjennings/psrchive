//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileWeightFunction.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/17 13:34:51 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileWeightFunction_h
#define __Pulsar_ProfileWeightFunction_h

#include "Pulsar/PhaseWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! PhaseWeight algorithms that compute profile baselines
  /*! This pure virtual base class defines the interface through which
      various baseline finding algorithms are accessed */
  class ProfileWeightFunction : public PhaseWeightFunction {

  public:

    //! Set the Profile from which the PhaseWeight will be derived
    virtual void set_Profile (const Profile*);

    //! Returns a PhaseWeight with the Profile attribute set
    virtual void get_weight (PhaseWeight& weight);

  protected:

    //! The Profile from which the PhaseWeight will be derived
    Reference::To<const Profile> profile;

    //! Derived classes implement the PhaseWeight calculation
    virtual void calculate (PhaseWeight& weight) = 0;

  };

}


#endif // !defined __Pulsar_BaselineWeightFunction_h
