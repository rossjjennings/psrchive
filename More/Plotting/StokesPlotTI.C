#include "Pulsar/StokesPlotterTI.h"
#include "Pulsar/FluxPlotterTI.h"

Pulsar::StokesPlotterTI::StokesPlotterTI (StokesPlotter* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlotterTI() );

  remove ("pol");

  add( &StokesPlotter::get_plot_values,
       &StokesPlotter::set_plot_values,
       "val", "Values to be plotted (IQUVLp)" );

  add( &StokesPlotter::get_plot_colours,
       &StokesPlotter::set_plot_colours,
       "sci", "PGPLOT colour index for each value" );

  add( &StokesPlotter::get_plot_lines,
       &StokesPlotter::set_plot_lines,
       "sls", "PGPLOT line style for each value" );
}
