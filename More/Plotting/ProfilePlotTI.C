#include "Pulsar/SinglePlotterTI.h"
#include "Pulsar/FluxPlotterTI.h"

Pulsar::SinglePlotterTI::SinglePlotterTI (SinglePlotter* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlotterTI() );

  add( &SinglePlotter::get_pol,
       &SinglePlotter::set_pol,
       "pol", "Polarization to plot" );
}
