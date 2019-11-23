//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/MeanPhase.h

#ifndef __Pulsar_MeanPhase_h
#define __Pulsar_MeanPhase_h

#include "Pulsar/ProfileShiftEstimator.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  //! Estimates phase shift using the weighted mean sine and cosine of phase
  class MeanPhase : public ProfileShiftEstimator
  {

  public:

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<MeanPhase> ("MPH"); }

    //! Return a copy constructed instance of self
    MeanPhase* clone () const { return new MeanPhase(*this); }

  };

}


#endif // !defined __Pulsar_MeanPhase_h
