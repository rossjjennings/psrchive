#include "Pulsar/PlotFrameSizeTI.h"
#include "Pulsar/PlotFrameTI.h"
#include "iopair.h"

Pulsar::PlotFrameSizeTI::PlotFrameSizeTI (PlotFrameSize* instance)
{
  if (instance)
    set_instance (instance);

  import ( PlotFrameTI() );

  add( &PlotFrameSize::get_bottom_left,
       &PlotFrameSize::set_bottom_left,
       "bl", "Normalized coordinates of bottom left frame corner" );

  add( &PlotFrameSize::get_top_right,
       &PlotFrameSize::set_top_right,
       "tr", "Normalized coordinates of top right frame corner" );
}
