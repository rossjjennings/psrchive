/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnStatistics.h"
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/Integration.h"

Pulsar::PolnStatistics::PolnStatistics () {}
Pulsar::PolnStatistics::~PolnStatistics () {}

//! Get the text interface to this
TextInterface::Parser* Pulsar::PolnStatistics::get_interface ()
{
  if (!stats)
    throw Error (InvalidState, "Pulsar::PolnStatistics::get_interface",
		 "PolnProfileStats not constructed");

  return stats->get_interface();
}

void Pulsar::PolnStatistics::setup (Statistics* st)
{
  if (!stats)
  {
    stats = new PolnProfileStats;
    stats -> set_stats (st->get_stats());
  }

  profile = Pulsar::get_Stokes (this->get_Integration(st), st->get_chan());

  stats->set_profile( profile );
}
