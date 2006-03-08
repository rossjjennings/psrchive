#include "Pulsar/PlotLabelTI.h"

Pulsar::PlotLabelTI::PlotLabelTI (PlotLabel* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotLabel::get_left,
       &PlotLabel::set_left,
       "l", "Left frame label" );

  add( &PlotLabel::get_centre,
       &PlotLabel::set_centre,
       "c", "Centre frame label" );

  add( &PlotLabel::get_right,
       &PlotLabel::set_right,
       "r", "Right frame label" );
}
