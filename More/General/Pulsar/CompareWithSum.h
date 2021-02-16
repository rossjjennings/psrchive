//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/CompareWithSum.h

#ifndef __CompareWithSum_h
#define __CompareWithSum_h

#include "Pulsar/CompareWith.h"

namespace Pulsar {

  //! Summarizes a comparison of each Profile with their sum
  /*! Uses a BinaryStatistic to compare each profile to the sum of profiles
    along the dimension defined by the compare method.
    Loops over all profiles along the dimenion defined by the primary method.
  */
  class CompareWithSum : public CompareWith
  {

  public:

    //! Compute the comparison summary over all dimensions
    void compute (ndArray<2,double>& result);
  };
}

#endif
