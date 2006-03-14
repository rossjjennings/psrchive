#include "Pulsar/MultiFrameTI.h"
#include "Pulsar/PlotFrameSizeTI.h"
#include "Pulsar/PlotScaleTI.h"

Pulsar::MultiFrameTI::MultiFrameTI (MultiFrame* instance)
{
  if (instance)
    set_instance (instance);

  if (instance->has_shared_x_scale())
    import ( "x", PlotScaleTI(), &MultiFrame::get_shared_x_scale );

  if (instance->has_shared_y_scale())
    import ( "y", PlotScaleTI(), &MultiFrame::get_shared_y_scale );

  import_filter = true;

  import ( "", std::string(), PlotFrameSizeTI(), &MultiFrame::get_frame );
}
