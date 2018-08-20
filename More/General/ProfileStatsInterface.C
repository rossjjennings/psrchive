/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatsInterface.h"
#include "Pulsar/PhaseWeightInterface.h"

#include "Pulsar/ProfileWeightFunction.h"

Pulsar::ProfileStats::Interface::Interface (ProfileStats* instance,
					    bool manage_estimators)
{
  if (instance)
    set_instance (instance);

  // cerr << "Pulsar::ProfileStats::Interface constructor" << endl;

  typedef PhaseWeight* (ProfileStats::*Method) (void);

  if (manage_estimators)
    add( &ProfileStats::get_onpulse_estimator,
	 &ProfileStats::set_onpulse_estimator,
	 &ProfileWeightFunction::get_interface,
	 "on", "On-pulse estimator" );

  import( "on", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_onpulse );

  if (manage_estimators)
    add( &ProfileStats::get_baseline_estimator,
	 &ProfileStats::set_baseline_estimator,
	 &ProfileWeightFunction::get_interface,
	 "off", "Off-pulse estimator" );

  import( "off", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_baseline );

  import( "all", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_all );
}


