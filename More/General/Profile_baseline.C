/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/BaselineWindow.h"

/*! This use of the Functor template implements the Strategy design
 pattern (ghjv94 p.315) for calculating the profile baseline mask. */

Functor< Pulsar::PhaseWeight* (const Pulsar::Profile*) >
Pulsar::Profile::baseline_strategy ( new Pulsar::BaselineWindow,
				     &Pulsar::BaselineEstimator::baseline );

//! Return a PhaseWeight mask with the baseline phase bins enabled
Pulsar::PhaseWeight* Pulsar::Profile::baseline () const try
{
  return baseline_strategy (this);
}
catch (Error& error) {
  throw error += "Pulsar::Profile::baseline";
}
