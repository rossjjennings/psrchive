#include "Pulsar/ProfilePlotterTI.h"

Pulsar::ProfilePlotterTI::ProfilePlotterTI (ProfilePlotter* instance)
{
  if (instance)
    set_instance (instance);

  add( &ProfilePlotter::get_scale,
       &ProfilePlotter::set_scale,
       "scale", "Scale on phase axis (ms,deg,rad)" );

  add( &ProfilePlotter::get_origin_norm,
       &ProfilePlotter::set_origin_norm,
       "origin", "Offset of origin on phase axis" );
}
