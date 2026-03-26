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

    //! The type of fit performed when subtracting the second profile from the first
    typedef enum { None, LeftToRight, RightToLeft } FitType;

    //! Set whether to subtract the best linear fit of the second profile from the first
    void set_fit (FitType f) { fit = f; }

  protected:

    //! Type of fit performed before subtracting the second profile from the first
    FitType fit = None;
  }; 

}

#endif
