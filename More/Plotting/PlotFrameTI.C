#include "Pulsar/PlotFrameTI.h"
#include "Pulsar/PlotAxisTI.h"
#include "Pulsar/PlotLabelTI.h"

Pulsar::PlotFrameTI::PlotFrameTI (PlotFrame* instance)
{
  import ( "x", PlotAxisTI(), &PlotFrame::get_x_axis );
  import ( "y", PlotAxisTI(), &PlotFrame::get_y_axis );
  import ( "lhi", PlotLabelTI(), &PlotFrame::get_label_over );
  import ( "llo", PlotLabelTI(), &PlotFrame::get_label_under );
}
