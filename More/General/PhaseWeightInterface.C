/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightInterface.h"

Pulsar::PhaseWeight::Interface::Interface (PhaseWeight* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseWeight::get_weight_sum,
       "count", "count of selected phase bins" );

  add( &PhaseWeight::get_weighted_sum,
       "sum", "sum of selected phase bins" );

  add( &PhaseWeight::get_min,
       "min", "minimum of selected phase bins" );

  add( &PhaseWeight::get_max,
       "max", "maximum of selected phase bins" );

  add( &PhaseWeight::get_avg,
       "avg", "average of selected phase bins" );

  add( &PhaseWeight::get_rms,
       "rms", "standard deviation" );

  add( &PhaseWeight::get_median,
       "med", "median of selected phase bins" );

  add( &PhaseWeight::get_median_difference,
       "mdm", "median difference from the median" );
}


