//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Logarithm.h

#ifndef __Pulsar_Logarithm_h
#define __Pulsar_Logarithm_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Compute the logarithm of the Profile 
  class Logarithm : public Transformation<Profile> {

  public:

    //! Default constructor
    Logarithm ();

    //! Destructor
    ~Logarithm ();

    //! Logarithm the given Profile
    void transform (Profile* profile);

  };

}

#endif
