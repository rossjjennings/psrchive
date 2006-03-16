#include "Pulsar/PlotAxis.h"

Pulsar::PlotAxis::Interface::Interface (PlotAxis* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotAxis::get_label,
       &PlotAxis::set_label,
       "lab", "Label to be drawn on axis" );

  add( &PlotAxis::get_pgbox_opt,
       &PlotAxis::set_pgbox_opt,
       "opt", "Options passed to PGBOX" );

  add( &PlotAxis::get_alternate,
       &PlotAxis::set_alternate,
       "alt", "Permit alternate scale on opposite axis" );

}
