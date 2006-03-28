/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DurationWeight.h"
#include "Pulsar/Integration.h"

//! Default constructor
Pulsar::DurationWeight::DurationWeight ()
{
  weight_absolute = false;
}

//! Set integration weights
void Pulsar::DurationWeight::weight (Integration* integration)
{
  double duration = integration->get_duration();
  unsigned nchan = integration->get_nchan();

  for (unsigned ichan=0; ichan < nchan; ichan++) {

    float chan_weight = integration->get_weight (ichan);

    // always ignore zapped channels
    if (!chan_weight)
      continue;

    if (weight_absolute)
      chan_weight = duration;
    else
      chan_weight *= duration;

    integration->set_weight (ichan, chan_weight);

  }
}
