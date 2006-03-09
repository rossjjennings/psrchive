#include "Pulsar/PhaseVsTimeTI.h"
#include "Pulsar/PhaseVsPlotterTI.h"

Pulsar::PhaseVsTimeTI::PhaseVsTimeTI (PhaseVsTime* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsTime::get_chan,
       &PhaseVsTime::set_chan,
       "chan", "Frequency channel to plot" );

  add( &PhaseVsTime::get_pol,
       &PhaseVsTime::set_pol,
       "pol", "Polarization to plot" );

  // import the interface of the base class
  import( PhaseVsPlotterTI() );



}
