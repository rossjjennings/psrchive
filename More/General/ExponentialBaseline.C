/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Pulsar::ExponentialBaseline::ExponentialBaseline ()
{
  threshold = 3.0;
}

void Pulsar::ExponentialBaseline::get_bounds (PhaseWeight& weight, 
					      float& lower, float& upper)
{
  weight.set_Profile (profile);

#ifdef _DEBUG
  cerr << "Pulsar::ExponentialBaseline::get_bounds mean=" << weight.get_mean()
       << " sigma=" << sqrt(weight.get_variance()) << endl;
#endif

  lower = 0.0;
  upper = threshold * weight.get_mean().get_value();
}
