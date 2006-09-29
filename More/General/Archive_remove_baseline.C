/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  If phase is not specified, this method calls
  Archive::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Archive::total)
  reaches a minimum.  This phase is then used to remove the baseline from
  each of the Integrations.
  */
void Pulsar::Archive::remove_baseline (float phase, float dc) try {
  
  if (phase < 0.0)
    phase = find_min_phase (dc);
  
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> remove_baseline (phase, dc);
  
}
catch (Error& error) {
  throw error += "Pulsar::Archive::remove_baseline";
}
