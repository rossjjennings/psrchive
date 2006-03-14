#include "Pulsar/PhaseScaleTI.h"
#include "Pulsar/PlotZoomTI.h"

Pulsar::PhaseScaleTI::PhaseScaleTI (PhaseScale* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseScale::get_scale,
       &PhaseScale::set_scale,
       "scale", "Scale on phase axis (turn,deg,rad,ms)" );

  add( &PhaseScale::get_origin_norm,
       &PhaseScale::set_origin_norm,
       "origin", "Offset of origin on phase axis" );
}
