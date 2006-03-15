#include "Pulsar/ProfilePlotTI.h"
#include "Pulsar/FluxPlotTI.h"

Pulsar::ProfilePlotTI::ProfilePlotTI (ProfilePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlotTI() );

  add( &ProfilePlot::get_plot_cal_transitions,
       &ProfilePlot::set_plot_cal_transitions,
       "cal", "Plot cal transitions (when applicable)" );
}
