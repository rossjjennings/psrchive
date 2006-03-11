#include "Pulsar/SinglePlotTI.h"
#include "Pulsar/FluxPlotTI.h"

Pulsar::SinglePlotTI::SinglePlotTI (SinglePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlotTI() );

  add( &SinglePlot::get_plot_cal_transitions,
       &SinglePlot::set_plot_cal_transitions,
       "cal", "Plot cal transitions (when applicable)" );
}
