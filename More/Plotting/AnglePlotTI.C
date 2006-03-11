#include "Pulsar/AnglePlotterTI.h"
#include "Pulsar/PhasePlotTI.h"

Pulsar::AnglePlotterTI::AnglePlotterTI (AnglePlotter* instance)
{
  if (instance)
    set_instance (instance);

  add( &AnglePlotter::get_subint,
       &AnglePlotter::set_subint,
       "subint", "Sub-integration to plot" );

  add( &AnglePlotter::get_chan,
       &AnglePlotter::set_chan,
       "chan", "Frequency channel to plot" );

  // import the interface of the base class
  import( PhasePlotTI() );

  add( &AnglePlotter::get_error_bars,
       &AnglePlotter::set_error_bars,
       "ebar", "Plot estimated error bars" );

  add( &AnglePlotter::get_threshold,
       &AnglePlotter::set_threshold,
       "cut", "Plot only points with R > cut * sigma_R" );

  add( &AnglePlotter::get_range,
       &AnglePlotter::set_range,
       "range", "Range of angles in plot (degrees)" );

}
