#include "Pulsar/PlotLabelTI.h"

Pulsar::PlotLabelTI::PlotLabelTI (PlotLabel* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotLabel::get_left,
       &PlotLabel::set_left,
       "l", "Left frame label" );


}
