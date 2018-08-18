//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/ProfileStrategies.h

#ifndef __Pulsar_ProfileStrategies_h
#define __Pulsar_ProfileStrategies_h

#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Manages the strategies that implement algorithms
  class Profile::Strategies : public Reference::Able
  {
  public:
    
    //! The implementation of the baseline finding algorithm
    virtual ProfileWeightFunction* baseline () = 0;

    //! The implementation of the on-pulse finding algorithm
    virtual ProfileWeightFunction* onpulse () = 0;

    //! The implementation of the signal-to-noise ratio calculation
    virtual SNRatioEstimator* snratio () = 0;
  };

  class DefaultStrategies : public Profile::Strategies
  {
    //! The default implementation of the baseline finding algorithm
    Reference::To<ProfileWeightFunction> baseline_strategy;

    //! The default implementation of the onpulse finding algorithm
    Reference::To<ProfileWeightFunction> onpulse_strategy;

    //! The default implementation of the snr method
    Reference::To<SNRatioEstimator> snratio_strategy;

  public:

    //! The default implementation of the baseline finding algorithm
    static Option< Reference::To<ProfileWeightFunction> > default_baseline;

    //! The default implementation of the onpulse finding algorithm
    static Option< Reference::To<ProfileWeightFunction> > default_onpulse;

    //! The default implementation of the snr method
    static Option< Reference::To<SNRatioEstimator> > default_snratio;

    //! The implementation of the baseline finding algorithm
    ProfileWeightFunction* baseline ();

    //! The implementation of the on-pulse finding algorithm
    ProfileWeightFunction* onpulse ();

    //! The implementation of the signal-to-noise ratio calculation
    SNRatioEstimator* snratio ();
  };
    
}

#endif
