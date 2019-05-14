/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/SNRatioEstimator.h"
#include "interface_stream.h"

using namespace Pulsar;
using namespace std;

//! Default constructor
StrategySet::Interface::Interface ( StrategySet* instance )
{
  if (instance)
    set_instance (instance);

  add( &StrategySet::onpulse,
       &StrategySet::set_onpulse,
       &ProfileWeightFunction::get_interface,
       "^on", "Install on-pulse estimator" );

  add( &StrategySet::baseline,
       &StrategySet::set_baseline,
       &ProfileWeightFunction::get_interface,
       "^off", "Install off-pulse estimator" );

  add( &StrategySet::snratio,
       &StrategySet::set_snratio,
       &SNRatioEstimator::get_interface,
       "^snr", "Install signal-to-noise ratio estimator" );
}
