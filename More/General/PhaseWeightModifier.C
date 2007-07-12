/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightModifier.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

//! Set the PhaseWeight from which the PhaseWeight will be derived
void Pulsar::PhaseWeightModifier::set_weight (const PhaseWeight* _weight)
{
  input_weight = _weight;
}

//! Returns a PhaseWeight with the Weight attribute set
void Pulsar::PhaseWeightModifier::get_weight (PhaseWeight& weight)
{
  Reference::To<PhaseWeight> output_weight = &weight;
  if (output_weight == input_weight)
    output_weight = new PhaseWeight (weight.get_nbin());

  calculate (output_weight);

  if (output_weight != &weight)
    weight = *output_weight;
}
