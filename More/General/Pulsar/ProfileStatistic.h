//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ProfileStatistic.h

#ifndef __ProfileStatistic_h
#define __ProfileStatistic_h

#include "UnaryStatistic.h"

namespace Pulsar {

  class Profile;

  //! Commmon statistics that can be derived from a pulse profile
  class ProfileStatistic : public Identifiable::Decorator
  {
  public:

    //! Create a new instance of ProfileStatistic based on name
    static ProfileStatistic* factory (const std::string& name);

    //! Returns a list of available ProfileStatistic children
    static const std::vector<ProfileStatistic*>& children ();

    //! Construct from a UnaryStatistic
    ProfileStatistic (UnaryStatistic*);

    //! Derived types define the value returned
    virtual double get (const Profile*);

    //! Derived types must also define clone method
    virtual ProfileStatistic* clone () const;

  private:

    Reference::To<UnaryStatistic> stat;

    //! thread-safe build for factory
    static void build (); 
  };

}

#endif

