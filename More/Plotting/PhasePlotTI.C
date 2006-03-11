#include "Pulsar/PhasePlotTI.h"

Pulsar::PhasePlotTI::PhasePlotTI (PhasePlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhasePlot::get_scale,
       &PhasePlot::set_scale,
       "scale", "Scale on phase axis (turn,deg,rad,ms)" );

  add( &PhasePlot::get_origin_norm,
       &PhasePlot::set_origin_norm,
       "origin", "Offset of origin on phase axis" );
}
