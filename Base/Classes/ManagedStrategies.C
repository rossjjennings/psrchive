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

ProfileWeightFunction* ManagedStrategies::baseline () const
{
  throw Error (InvalidState, "ManagedStrategies::baseline",
	       "not implemented");
}

ProfileWeightFunction* ManagedStrategies::onpulse () const
{
  throw Error (InvalidState, "ManagedStrategies::onpulse",
	       "not implemented");
}

SNRatioEstimator* ManagedStrategies::snratio () const
{
  throw Error (InvalidState, "ManagedStrategies::snratio",
	       "not implemented");
}

WidthEstimator* ManagedStrategies::width () const
{
  throw Error (InvalidState, "ManagedStrategies::width",
               "not implemented");
}

