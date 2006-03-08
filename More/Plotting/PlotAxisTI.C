#include "Pulsar/PlotAxisTI.h"

Pulsar::PlotAxisTI::PlotAxisTI (PlotAxis* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotAxis::get_label,
       &PlotAxis::set_label,
       "lab", "Label to be drawn on axis" );

  add( &PlotAxis::get_alternate,
       &PlotAxis::set_alternate,
       "alt", "Permit alternate scale on opposite axis" );

  add( &PlotAxis::get_min_norm,
       &PlotAxis::set_min_norm,
       "fmin", "Normalized minimum value" );

  add( &PlotAxis::get_max_norm,
       &PlotAxis::set_max_norm,
       "fmax", "Normalized maximum value" );

  add( &PlotAxis::get_buf_norm,
       &PlotAxis::set_buf_norm,
       "fbuf", "Normalized buffer space" );

  add( &PlotAxis::get_pgbox_opt,
       &PlotAxis::set_pgbox_opt,
       "opt", "Options passed to PGBOX" );

}
