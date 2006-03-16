#include "Pulsar/Poincare.h"

Pulsar::Poincare::Interface::Interface (Poincare* instance)
{
  if (instance)
    set_instance (instance);

  add( &Poincare::get_subint,
       &Poincare::set_subint,
       "subint", "Sub-integration to plot" );

  add( &Poincare::get_chan,
       &Poincare::set_chan,
       "chan", "Frequency channel to plot" );

  import( "ph", PlotScale::Interface(), &Poincare::get_phase_scale );

  add( &Poincare::get_longitude,
       &Poincare::set_longitude,
       "long", "Camera longitude" );

  add( &Poincare::get_latitude,
       &Poincare::set_latitude,
       "lat", "Camera latitude" );

  add( &Poincare::get_animate,
       &Poincare::set_animate,
       "anim", "Animate by rotating axes" );
}
