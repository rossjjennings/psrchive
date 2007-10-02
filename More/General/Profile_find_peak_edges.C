/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/PeakEdgesInterpreter.h"
#include "Error.h"

//! The default implementation of the baseline finding algorithm
Functor< std::pair<int,int> (const Pulsar::Profile*) > 
Pulsar::Profile::peak_edges_strategy;

/*!  
  The PeakEdgesInterpreter class sets the peak_edges_strategy
  attribute according to commands specified either in the
  configuration file or via the psrsh interpreter.  It enables
  convenient experimentation with the peak edges estimation algorithm.
*/
static Pulsar::Option<CommandParser>
cfg( new Pulsar::PeakEdgesInterpreter, "Profile::peak_edges", "cumulative" );


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_peak_edges
//
void Pulsar::Profile::find_peak_edges (int& rise, int& fall) const
try {

  std::pair<int,int> edges = peak_edges_strategy (this);
  rise = edges.first;
  fall = edges.second;

}
catch (Error& error) {
  throw error += "Pulsar::Profile::find_peak_edges";
}
