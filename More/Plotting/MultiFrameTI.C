#include "Pulsar/MultiFrameTI.h"
#include "Pulsar/PlotFrameSizeTI.h"
#include "Pulsar/PlotZoomTI.h"

Pulsar::MultiFrameTI::MultiFrameTI (MultiFrame* instance)
{
  if (instance)
    set_instance (instance);

  if (instance->has_shared_x_zoom())
    import ( "x", PlotZoomTI(), &MultiFrame::get_shared_x_zoom );

  if (instance->has_shared_y_zoom())
    import ( "y", PlotZoomTI(), &MultiFrame::get_shared_y_zoom );

  import_filter = true;

  import ( "", std::string(), PlotFrameSizeTI(), &MultiFrame::get_frame );
}
