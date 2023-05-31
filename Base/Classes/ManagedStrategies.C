/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/Integration.h"

#include "debug.h"

using namespace Pulsar;
using namespace std;

ManagedStrategies::ManagedStrategies (Integration* c)
{
  DEBUG("ManagedStrategies ctor this=" << this << " container=" << (void*) c);
  container = c;
}

ManagedStrategies::ManagedStrategies (const ManagedStrategies& that)
{
  container = that.container;
  DEBUG("ManagedStrategies copy ctor this=" << this << " container=" << (void*) container.ptr());
}

ManagedStrategies::~ManagedStrategies ()
{
  DEBUG("ManagedStrategies dtor this=" << this << " container=" << (void*) container.ptr());
}

//! Return the container
Integration* ManagedStrategies::get_container()
{
  return container;
}

ManagedStrategies* ManagedStrategies::clone () const
{
  return new ManagedStrategies (*this);
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

