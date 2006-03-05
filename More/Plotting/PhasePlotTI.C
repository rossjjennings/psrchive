#include "Pulsar/ProfilePlotterTI.h"

Pulsar::ProfilePlotterTI::ProfilePlotterTI (ProfilePlotter* instance)
{
  if (instance)
    set_instance (instance);

  add( &ProfilePlotter::get_subint,
       &ProfilePlotter::set_subint,
       "subint", "Sub-integration to plot" );

  add( &ProfilePlotter::get_pol,
       &ProfilePlotter::set_pol,
       "pol", "Polarization to plot" );

  add( &ProfilePlotter::get_chan,
       &ProfilePlotter::set_chan,
       "chan", "Frequency channel to plot" );

  add( &ProfilePlotter::get_min_phase,
       &ProfilePlotter::set_min_phase,
       "ph0", "Minimum phase in plot" );

  add( &ProfilePlotter::get_max_phase,
       &ProfilePlotter::set_max_phase,
       "ph1", "Maximum phase in plot" );

  add( &ProfilePlotter::get_min_fraction,
       &ProfilePlotter::set_min_fraction,
       "f0", "Fractional height of minimum" );

  add( &ProfilePlotter::get_max_fraction,
       &ProfilePlotter::set_max_fraction,
       "f1", "Fractional height of maximum" );

  add( &ProfilePlotter::get_plot_error_box,
       &ProfilePlotter::set_plot_error_box,
       "ebox", "Plot error box" );

  add( &ProfilePlotter::get_plot_axes,
       &ProfilePlotter::set_plot_axes,
       "axes", "Plot axes" );

  add( &ProfilePlotter::get_x_enumerate,
       &ProfilePlotter::set_x_enumerate,
       "xnum", "Enumerate the x-axis" );

  add( &ProfilePlotter::get_y_enumerate,
       &ProfilePlotter::set_y_enumerate,
       "ynum", "Enumerate the y-axis" );

  add( &ProfilePlotter::get_x_label,
       &ProfilePlotter::set_x_label,
       "xlab", "Label the x-axis" );

  add( &ProfilePlotter::get_y_label,
       &ProfilePlotter::set_y_label,
       "ylab", "Label the y-axis" );

#if 0
  add( &ProfilePlotter::,
       &ProfilePlotter::,
       "", "" );
#endif

}
