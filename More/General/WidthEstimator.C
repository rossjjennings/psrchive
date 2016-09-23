/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WidthEstimator.h"
#include "interface_factory.h"

#include "Pulsar/PhaseWidth.h"
// #include "Pulsar/MTMWidth.h"

using namespace std;

Pulsar::WidthEstimator*
Pulsar::WidthEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<WidthEstimator> > instances;

  instances.push_back( new PhaseWidth );

  return TextInterface::factory<WidthEstimator> (instances, name_parse);
}

