/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

using namespace std;

/*!
  If phase is not specified, this method calls
  Archive::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Archive::total)
  reaches a minimum.  This phase is then used to remove the baseline from
  each of the Integrations.
  */

void Pulsar::Archive::remove_baseline (float phase, float duty_cycle) try {
  
  if (phase < 0.0)
    phase = find_min_phase (duty_cycle);
  
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> remove_baseline (phase, duty_cycle);
  
}
catch (Error& error) {
  throw error += "Pulsar::Archive::remove_baseline";
}
