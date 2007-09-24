/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/PeakCumulative.h"
#include "Error.h"

Reference::To<Pulsar::RiseFall> Pulsar::Profile::peak_edges;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_peak_edges
//
void Pulsar::Profile::find_peak_edges (int& rise, int& fall) const
try {

  if (!peak_edges)
    peak_edges = new Pulsar::PeakCumulative;

  peak_edges->set_Profile (this);
  peak_edges->get_indeces (rise, fall);

}
catch (Error& error) {
  throw error += "Pulsar::Profile::find_peak_edges";
}
