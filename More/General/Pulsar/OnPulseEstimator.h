//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/OnPulseEstimator.h

#ifndef __Pulsar_OnPulseEstimator_h
#define __Pulsar_OnPulseEstimator_h

#include "Pulsar/ProfileWeightFunction.h"

namespace Pulsar
{
  //! ProfileWeight algorithms that compute on-pulse phase bins
  class OnPulseEstimator : public ProfileWeightFunction
  {
  public:

    //! Return a copy constructed instance of self
    virtual OnPulseEstimator* clone () const = 0;
  };
}

#endif // !defined __Pulsar_OnPulseEstimator_h
