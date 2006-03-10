#include "Pulsar/MultiFrameTI.h"
#include "Pulsar/PlotFrameSizeTI.h"
#include "Pulsar/PlotAxisTI.h"

Pulsar::MultiFrameTI::MultiFrameTI (MultiFrame* instance)
{
  if (instance)
    set_instance (instance);

  if (instance->has_shared_x_axis()) {
    import ( "x", PlotAxisTI(), &MultiFrame::get_shared_x_axis );
    remove ( "x:lab" );
    remove ( "x:opt" );
  }

  if (instance->has_shared_y_axis()) {
    import ( "y", PlotAxisTI(), &MultiFrame::get_shared_y_axis );
    remove ( "y:lab" );
    remove ( "y:opt" );
  }

  import_filter = true;

  import ( "frame", std::string(), PlotFrameSizeTI(), &MultiFrame::get_frame );
}
