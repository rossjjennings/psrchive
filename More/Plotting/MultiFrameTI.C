#include "Pulsar/MultiFrameTI.h"
#include "Pulsar/PlotFrameSizeTI.h"

Pulsar::MultiFrameTI::MultiFrameTI (MultiFrame* instance)
{
  if (instance)
    set_instance (instance);

  import ( "frame", std::string(), PlotFrameSizeTI(), &MultiFrame::get_frame );
}
