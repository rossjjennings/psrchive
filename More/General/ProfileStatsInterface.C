/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatsInterface.h"
#include "Pulsar/PhaseWeightInterface.h"

#include "Pulsar/ProfileWeightFunction.h"

#include <iostream>
using namespace std;

Pulsar::ProfileStats::Interface::Interface (ProfileStats* instance,
					    bool manage_estimators)
{
  if (instance)
    set_instance (instance);

  // cerr << "Pulsar::ProfileStats::Interface constructor" << endl;

  typedef PhaseWeight* (ProfileStats::*Method) (void);

  add( &ProfileStats::get_include_estimator,
       &ProfileStats::set_include_estimator,
       &ProfileWeightFunction::get_interface,
       "include", "Estimator of included bins" );

  add( &ProfileStats::get_exclude_estimator,
       &ProfileStats::set_exclude_estimator,
       &ProfileWeightFunction::get_interface,
       "exclude", "Estimator of excluded bins" );

  if (manage_estimators)
    add( &ProfileStats::get_onpulse_estimator,
	 &ProfileStats::set_onpulse_estimator,
	 &ProfileWeightFunction::get_interface,
	 "on", "On-pulse estimator" );

  import( "on", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_onpulse );

  if (manage_estimators)
    add( &HasBaselineEstimator::get_baseline_estimator,
	 &HasBaselineEstimator::set_baseline_estimator,
	 &ProfileWeightFunction::get_interface,
	 "off", "Off-pulse estimator" );

  import( "off", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_baseline );

  import( "all", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_all );

  add( &ProfileStats::get_pulse_width,
       &ProfileStats::set_pulse_width_estimator,
       &ProfileStats::get_pulse_width_interface,
       "width", "Pulse width (turns)" );

  add( &ProfileStats::get_snr,
       &ProfileStats::set_snr_estimator,
       &ProfileStats::get_snr_interface,
       "snr", "Signal-to-noise ratio" );
}


