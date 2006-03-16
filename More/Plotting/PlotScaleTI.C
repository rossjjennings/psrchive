#include "Pulsar/PlotScale.h"
#include "iopair.h"

Pulsar::PlotScale::Interface::Interface (PlotScale* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotScale::get_range_norm,
       &PlotScale::set_range_norm,
       "range", "Normalized value range" );

  add( &PlotScale::get_buf_norm,
       &PlotScale::set_buf_norm,
       "buf", "Normalized buffer space" );
}
