/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/Integration.h"

using namespace Pulsar;

ManagedStrategies::ManagedStrategies (Integration* c)
{
  container = c;
}

//! Return the container
Integration* ManagedStrategies::get_container()
{
  return container;
}

ProfileWeightFunction* ManagedStrategies::baseline ()
{
  throw Error (InvalidState, "ManagedStrategies::baseline",
	       "not implemented");
}

ProfileWeightFunction* ManagedStrategies::onpulse ()
{
  throw Error (InvalidState, "ManagedStrategies::onpulse",
	       "not implemented");
}

SNRatioEstimator* ManagedStrategies::snratio ()
{
  throw Error (InvalidState, "ManagedStrategies::snratio",
	       "not implemented");
}

