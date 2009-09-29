/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ShiftEstimator.h"

//! Construct a new ShiftEstimator from a string
Pulsar::ShiftEstimator* 
Pulsar::ShiftEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<ShiftEstimator> > instances;

#if 0
  instances.push_back( new ExponentialBaseline );
  instances.push_back( new GaussianBaseline );
  instances.push_back( new BaselineWindow );
#endif

  return TextInterface::factory<ShiftEstimator> (instances, name_parse);
}
