/***************************************************************************
 *
 *   Copyright (C) 2007 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/ProfileWeightFunction.h"

#include <iostream>
using namespace std;

//! Return a PhaseWeight mask with the baseline phase bins enabled
Pulsar::PhaseWeight* Pulsar::Profile::baseline () const try
{
  if (verbose)
    cerr << "Profile::baseline strategy=" 
         << tostring(get_strategy()->baseline()) << endl;

  return get_strategy()->baseline()->operate (this);
}
catch (Error& error)
{
  throw error += "Pulsar::Profile::baseline";
}
