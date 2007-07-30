/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"

Pulsar::PhaseWeight*
Pulsar::BaselineEstimator::baseline (const Profile* profile)
{
  Reference::To<PhaseWeight> weight = new PhaseWeight;
  set_Profile( profile );
  get_weight( *weight );
  return weight.release();
}

