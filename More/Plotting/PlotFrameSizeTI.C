#include "Pulsar/PlotFrameSizeTI.h"
#include "Pulsar/PlotFrameTI.h"
#include "iopair.h"

Pulsar::PlotFrameSizeTI::PlotFrameSizeTI (PlotFrameSize* instance)
{
  if (instance)
    set_instance (instance);

  import ( PlotFrameTI() );

  add( &PlotFrameSize::get_x_range,
       &PlotFrameSize::set_x_range,
       "vpx", "Normalized coordinates of viewport x-axis" );

  add( &PlotFrameSize::get_y_range,
       &PlotFrameSize::set_y_range,
       "vpy", "Normalized coordinates of viewport y-axis" );
}
