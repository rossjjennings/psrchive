//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Subtract.h

#ifndef __Pulsar_Subtract_h
#define __Pulsar_Subtract_h

#include "Pulsar/Combination.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Computes the difference between two profiles
  class Subtract : public Combination<Profile>
  {

  public:

    void transform (Profile*);

  }; 

}

#endif
