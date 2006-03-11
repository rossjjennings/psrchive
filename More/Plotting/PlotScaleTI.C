#include "Pulsar/PlotZoomTI.h"
#include "iopair.h"

Pulsar::PlotZoomTI::PlotZoomTI (PlotZoom* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotZoom::get_range_norm,
       &PlotZoom::set_range_norm,
       "range", "Normalized value range" );

  add( &PlotZoom::get_buf_norm,
       &PlotZoom::set_buf_norm,
       "buf", "Normalized buffer space" );
}
