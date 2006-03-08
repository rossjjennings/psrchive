#include "Pulsar/SinglePlotterTI.h"
#include "Pulsar/FluxPlotterTI.h"

Pulsar::SinglePlotterTI::SinglePlotterTI (SinglePlotter* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlotterTI() );

  add( &SinglePlotter::get_plot_cal_transitions,
       &SinglePlotter::set_plot_cal_transitions,
       "cal", "Plot cal transitions (when applicable)" );
}
