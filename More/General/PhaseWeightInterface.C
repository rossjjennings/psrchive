/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightInterface.h"
#include "Pulsar/PhaseWeightStatistic.h"
#include "Functor.h"

Pulsar::PhaseWeight::Interface::Interface (PhaseWeight* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseWeight::get_nonzero_weight_count,
       "count", "Count of selected phase bins" );

  add( &PhaseWeight::get_start_index,
	"start", "Start of the phase region" );

  add( &PhaseWeight::get_end_index,
	"end", "End of the phase region" );

  auto stats = PhaseWeightStatistic::children ();

  for (auto element : stats)
    add( Functor< double(const PhaseWeight*) > (element,
                                                &PhaseWeightStatistic::get),
        element->get_identity().c_str(), 
        element->get_description().c_str() );
}


