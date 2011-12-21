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

using namespace std;

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

  profile = Pulsar::get_Stokes (get_Integration(st), st->get_chan());

  PhaseWeight* baseline = stats->get_stats()->get_baseline();

  Reference::To<PolnProfile> p = profile->clone();
  for (unsigned i=0; i<4; i++)
  {
    Profile* s = p->get_Profile(i);
    baseline->set_Profile(s);
    s->offset( -baseline->get_mean().get_value() );
  }
  profile = p;

  stats->set_profile( profile );
}

