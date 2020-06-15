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

#include "Identifiable.h"

namespace Pulsar {

  class Profile;

  //! Commmon statistics that can be derived from a pulse profile
  class ProfileStatistic : public Identifiable
  {
  public:

    //! Create a new instance of ProfileStatistic based on name
    static ProfileStatistic* factory (const std::string& name);

    //! Construct with a name and description
    ProfileStatistic (const std::string& name,
                      const std::string& description);

    //! Derived types define the value returned
    virtual double get (const Profile*) = 0;

    //! Derived types must also define clone method
    virtual ProfileStatistic* clone () const = 0;

  private:

    //! thread-safe build for factory
    static void build (); 
  };

}

#endif

