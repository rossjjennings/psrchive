#include "Pulsar/FluxPlotterTI.h"
#include "Pulsar/ProfilePlotterTI.h"

Pulsar::FluxPlotterTI::FluxPlotterTI (FluxPlotter* instance)
{
  if (instance)
    set_instance (instance);

  add( &FluxPlotter::get_subint,
       &FluxPlotter::set_subint,
       "subint", "Sub-integration to plot" );

  add( &FluxPlotter::get_chan,
       &FluxPlotter::set_chan,
       "chan", "Frequency channel to plot" );

  add( &FluxPlotter::get_pol,
       &FluxPlotter::set_pol,
       "pol", "Polarization to plot" );

  import( ProfilePlotterTI() );

  add( &FluxPlotter::get_plot_error_box,
       &FluxPlotter::set_plot_error_box,
       "ebox", "Plot error box" );

  add( &FluxPlotter::get_plot_histogram,
       &FluxPlotter::set_plot_histogram,
       "hist", "Plot histogram style" );
}
