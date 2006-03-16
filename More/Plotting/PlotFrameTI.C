#include "Pulsar/PlotFrame.h"
#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

Pulsar::PlotFrame::Interface::Interface (PlotFrame* instance)
{
  if (instance)
    set_instance (instance);

  import ( "x", PlotScale::Interface(), &PlotFrame::get_x_scale );
  import ( "y", PlotScale::Interface(), &PlotFrame::get_y_scale );

  import ( "x", PlotAxis::Interface(), &PlotFrame::get_x_axis );
  import ( "y", PlotAxis::Interface(), &PlotFrame::get_y_axis );

  import ( "above", PlotLabel::Interface(), &PlotFrame::get_label_above );
  import ( "below", PlotLabel::Interface(), &PlotFrame::get_label_below );

  add( &PlotFrame::get_label_offset,
       &PlotFrame::set_label_offset,
       "laboff", "Character height between label and frame" );

  add( &PlotFrame::get_label_spacing,
       &PlotFrame::set_label_spacing,
       "labsep", "Character height between label rows" );
}
