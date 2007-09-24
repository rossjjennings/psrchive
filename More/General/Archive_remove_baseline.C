/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

Pulsar::PhaseWeight* Pulsar::Archive::baseline () const
{
  Reference::To<const Archive> total = this->total();
  return total->get_Profile(0,0,0)->baseline ();
}

/*!
  If phase is not specified, this method calls
  Archive::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Archive::total)
  reaches a minimum.  This phase is then used to remove the baseline from
  each of the Integrations.
  */

void Pulsar::Archive::remove_baseline () try {

  Reference::To<PhaseWeight> baseline = this->baseline();
  
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> remove_baseline (baseline);
  
}
catch (Error& error) {
  throw error += "Pulsar::Archive::remove_baseline";
}
