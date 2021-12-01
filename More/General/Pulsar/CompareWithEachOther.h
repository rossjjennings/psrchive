//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/CompareWithEachOther.h

#ifndef __CompareWithEachOther_h
#define __CompareWithEachOther_h

#include "Pulsar/CompareWith.h"

class UnaryStatistic;

namespace Pulsar {

  //! Summarizes a comparison of each Profile with every other Profile
  /*! Uses a BinaryStatistic to compare a profile with every other Profile
    along the dimension defined by the compare method.
    Uses a UnaryStatistic to summarize the results.
    Loops over all profiles along the dimenion defined by the primary method.
  */
  class CompareWithEachOther : public CompareWith
  {
    //! Two-dimensional array to store values
    ndArray<2,double> temp;

    //! Used to perform summary
    Reference::To<UnaryStatistic> summary;
    
    //! Estimates of the standard deviations of each profile
    std::vector<double> rms;

    //! Compute the comparison summary for primary dimension
    void compute (unsigned iprimary, ndArray<2,double>& result);
    
  public:

    CompareWithEachOther ();

    //! Used to summarize comparisons
    void set_summary (UnaryStatistic*);
  };
}

#endif
