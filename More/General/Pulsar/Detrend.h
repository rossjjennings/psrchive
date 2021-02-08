//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Detrend.h

#ifndef __Pulsar_Detrend_h
#define __Pulsar_Detrend_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile differentiation algorithm
  class Detrend : public Transformation<Profile> {

  public:

    //! Default constructor
    Detrend ();

    //! Destructor
    ~Detrend ();

    //! Detrend the given Profile
    void transform (Profile* profile);

  };

}

#endif
