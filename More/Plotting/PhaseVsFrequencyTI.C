#include "Pulsar/PhaseVsFrequencyTI.h"
#include "Pulsar/PhaseVsPlotTI.h"

Pulsar::PhaseVsFrequencyTI::PhaseVsFrequencyTI (PhaseVsFrequency* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsFrequency::get_subint,
       &PhaseVsFrequency::set_subint,
       "subint", "Sub-integration to plot" );

  add( &PhaseVsFrequency::get_pol,
       &PhaseVsFrequency::set_pol,
       "pol", "Polarization to plot" );

  // import the interface of the base class
  import( PhaseVsPlotTI() );



}
