//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/ChiSquared.h

#include "BinaryStatistic.h"

namespace BinaryStatistics
{
  class ChiSquared : public BinaryStatistic
  {
    bool robust_linear_fit;
    double max_zap_fraction;
 
  public:
    ChiSquared ();
    double get (const std::vector<double>&, const std::vector<double>&);
    ChiSquared* clone () const { return new ChiSquared; }
  };
}
