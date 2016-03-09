/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsMore.h"
#include "Pulsar/MoreScale.h"
#include "Pulsar/MoreProfiles.h"
#include "Pulsar/Archive.h"

#include <iostream>
using namespace std;

Pulsar::PhaseVsMore::PhaseVsMore ()
{
  // PhaseVsPlot gets row information from y_scale
  get_frame()->set_y_scale( new MoreScale );

  isubint = 0;
  ichan = 0;
}

TextInterface::Parser* Pulsar::PhaseVsMore::get_interface ()
{
  return new Interface (this);
}

std::string Pulsar::PhaseVsMore::get_ylabel (const Archive* data)
{
  return "Alternate";
}

void Pulsar::PhaseVsMore::prepare (const Archive *data)
{
  PhaseVsPlot::prepare (data);
  profile = data->get_Profile(isubint, 0, ichan);
}

const Pulsar::Profile*
Pulsar::PhaseVsMore::get_Profile (const Archive* data, unsigned index)
{
  return profile->get<MoreProfiles>()->get_Profile(index);
}

Pulsar::PhaseVsMore::Interface::Interface (PhaseVsMore* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsMore::get_subint,
       &PhaseVsMore::set_subint,
       "subint", "Sub-integration to plot" );

  add( &PhaseVsMore::get_chan,
       &PhaseVsMore::set_chan,
       "chan", "More channel to plot" );

  // import the interface of the base class
  import( PhaseVsPlot::Interface() );
}
