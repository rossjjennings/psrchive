#include "Pulsar/ProfilePlot.h"
#include "Pulsar/FluxPlot.h"

Pulsar::ProfilePlot::Interface::Interface (ProfilePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlot::Interface() );

  add( &ProfilePlot::get_plot_cal_transitions,
       &ProfilePlot::set_plot_cal_transitions,
       "cal", "Plot cal transitions (when applicable)" );
}
