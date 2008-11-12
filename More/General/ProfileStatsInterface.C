/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatsInterface.h"
#include "Pulsar/PhaseWeightInterface.h"

Pulsar::ProfileStats::Interface::Interface (ProfileStats* instance)
{
  if (instance)
    set_instance (instance);

  typedef PhaseWeight* (ProfileStats::*Method)(void);

  import ( "on", PhaseWeight::Interface(), 
	   (Method) &ProfileStats::get_on_pulse );

  import ( "off", PhaseWeight::Interface(), 
	   (Method) &ProfileStats::get_baseline );
}


