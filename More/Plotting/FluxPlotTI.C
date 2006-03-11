#include "Pulsar/FluxPlotTI.h"
#include "Pulsar/PhasePlotTI.h"

Pulsar::FluxPlotTI::FluxPlotTI (FluxPlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &FluxPlot::get_subint,
       &FluxPlot::set_subint,
       "subint", "Sub-integration to plot" );

  add( &FluxPlot::get_chan,
       &FluxPlot::set_chan,
       "chan", "Frequency channel to plot" );

  add( &FluxPlot::get_pol,
       &FluxPlot::set_pol,
       "pol", "Polarization to plot" );

  import( PhasePlotTI() );

  add( &FluxPlot::get_plot_error_box,
       &FluxPlot::set_plot_error_box,
       "ebox", "Plot error box" );

  add( &FluxPlot::get_plot_histogram,
       &FluxPlot::set_plot_histogram,
       "hist", "Plot histogram style" );
}
