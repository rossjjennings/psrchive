#include "Pulsar/StokesPlotTI.h"
#include "Pulsar/FluxPlotTI.h"

Pulsar::StokesPlotTI::StokesPlotTI (StokesPlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlotTI() );

  remove ("pol");

  add( &StokesPlot::get_plot_values,
       &StokesPlot::set_plot_values,
       "val", "Values to be plotted (IQUVLp)" );

  add( &StokesPlot::get_plot_colours,
       &StokesPlot::set_plot_colours,
       "sci", "PGPLOT colour index for each value" );

  add( &StokesPlot::get_plot_lines,
       &StokesPlot::set_plot_lines,
       "sls", "PGPLOT line style for each value" );
}
