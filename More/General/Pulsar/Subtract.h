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

    //! subtract the best linear fit of the second profile from the first
    bool subtract_linear_fit = false;

  public:

    void transform (Profile*);

    //! Set whether to subtract the best linear fit of the second profile from the first
    void set_linear_fit (bool fit) { subtract_linear_fit = fit; }

  }; 

}

#endif
