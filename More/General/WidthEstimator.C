/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WidthEstimator.h"
#include "Pulsar/Profile.h"
#include "interface_factory.h"

#include "Pulsar/PhaseWidth.h"
// #include "Pulsar/MTMWidth.h"

using namespace std;

Phase::Value Pulsar::WidthEstimator::get_width (const Profile* profile)
{
  Phase::Value width;
  width.set_value( get_width_turns(profile) );
  width.set_nbin( profile->get_nbin() );
  return width;
}

Pulsar::WidthEstimator*
Pulsar::WidthEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<WidthEstimator> > instances;

  instances.push_back( new PhaseWidth );

  return TextInterface::factory<WidthEstimator> (instances, name_parse);
}

