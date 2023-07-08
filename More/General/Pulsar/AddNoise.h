//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/AddNoise.h

#ifndef __Pulsar_AddNoise_h
#define __Pulsar_AddNoise_h

#include "Pulsar/Transformation.h"
#include "BoxMuller.h"

namespace Pulsar {

  class Profile;

  //! Compute the logarithm of the Profile 
  class AddNoise : public Transformation<Profile>
  {
    BoxMuller gasdev;
    double sigma;

  public:

    //! Default constructor
    AddNoise (double sigma);

    //! Destructor
    ~AddNoise ();

    //! AddNoise the given Profile
    void transform (Profile* profile);

  };

}

#endif
