/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Profile.h"
#include "Pulsar/BaselineWindow.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_min_phase
//
/*! Returns the centre phase of the region with minimum mean
  \param duty_cycle width of the region over which the mean is calculated
 */
float Pulsar::Profile::find_min_phase (float duty_cycle) const try {

  if (verbose)
    cerr << "Pulsar::Profile::find_min_phase" << endl;

  BaselineWindow mean;
  mean.set_duty_cycle (duty_cycle);
  mean.set_find_minimum ();
  return mean.find_phase (nbin, amps);
}
catch (Error& error) {
  throw error += "Pulsar::Profile::find_min_phase";
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_max_phase
//
/*! Returns the centre phase of the region with maximum mean
  \param duty_cycle width of the region over which the mean is calculated
 */
float Pulsar::Profile::find_max_phase (float duty_cycle) const try {

  if (verbose)
    cerr << "Pulsar::Profile::find_max_phase" << endl;
  
  BaselineWindow mean;
  mean.set_duty_cycle (duty_cycle);
  mean.set_find_maximum ();
  return mean.find_phase (nbin, amps);
}
catch (Error& error) {
  throw error += "Pulsar::Profile::find_max_phase";
}

