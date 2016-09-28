//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ProfileWeightStatic.h

#ifndef __Pulsar_ProfileWeightStatic_h
#define __Pulsar_ProfileWeightStatic_h

#include "Pulsar/ProfileWeightFunction.h"
#include "PhaseRange.h"

namespace Pulsar
{
  //! Set the Profile weights to a user-specified value
  class ProfileWeightStatic : public ProfileWeightFunction
  {

  public:

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Set the Profile from which the PhaseWeight will be derived
    void set_Profile (const Profile*);

    //! Return a copy constructed instance of self
    ProfileWeightStatic* clone () const;

    //! Set the profile phase bins
    void set_range (const Phase::Ranges& r) { range = r; }
    Phase::Ranges get_range () const { return range; }

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

    Phase::Ranges range;
  };

}

#endif
