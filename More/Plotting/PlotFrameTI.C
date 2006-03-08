#include "Pulsar/PlotFrameTI.h"
#include "Pulsar/PlotAxisTI.h"
#include "Pulsar/PlotLabelTI.h"

Pulsar::PlotFrameTI::PlotFrameTI (PlotFrame* instance)
{
  if (instance)
    set_instance (instance);

  import ( "x", PlotAxisTI(), &PlotFrame::get_x_axis );
  import ( "y", PlotAxisTI(), &PlotFrame::get_y_axis );
  import ( "above", PlotLabelTI(), &PlotFrame::get_label_above );
  import ( "below", PlotLabelTI(), &PlotFrame::get_label_below );

  add( &PlotFrame::get_label_offset,
       &PlotFrame::set_label_offset,
       "laboff", "Character height between label and frame" );

  add( &PlotFrame::get_label_spacing,
       &PlotFrame::set_label_spacing,
       "labsep", "Character height between label rows" );
}
