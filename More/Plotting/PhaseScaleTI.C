/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseScale.h"
#include "Pulsar/PlotScale.h"
#include "pairutil.h"

Pulsar::PhaseScale::Interface::Interface (PhaseScale* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotScale::get_index_range,
       &PlotScale::set_index_range,
       "bin", "Phase bin index range" );

  add( &PhaseScale::get_units,
       &PhaseScale::set_units,
       "unit", "Units on phase axis (turn,deg,rad,ms,bin)" );

  add( &PhaseScale::get_origin,
       &PhaseScale::set_origin,
       "origin", "Offset of origin on phase axis" );
}
