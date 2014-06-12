/***************************************************************************
 *
 *   Copyright (C) 2006 - 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/TimeScale.h"

Pulsar::PhaseVsTime::Interface::Interface (PhaseVsTime* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsTime::get_chan,
       &PhaseVsTime::set_chan,
       "chan", "Frequency channel to plot" );

  add( &PhaseVsTime::get_pol,
       &PhaseVsTime::set_pol,
       "pol", "Polarization to plot" );

  import ( "y", TimeScale::Interface(), &PhaseVsTime::get_y_scale );

  // import the interface of the base class
  import( PhaseVsPlot::Interface() );
}
