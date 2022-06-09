/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/Archive.h"

#include <iostream>
using namespace std;

void Pulsar::Integration::orphan ()
{
  if (orphaned)
    return;

  if (!parent)
    throw Error (InvalidState, "Pulsar::Integration::orphan",
                 "not already orphaned and no parent");

  if (verbose)
    cerr << "Pulsar::Integration::orphan from parent" << endl;

  orphaned = new Meta(parent);
  orphaned->set_strategy ( parent->get_strategy()->clone() );

  parent = 0;
}

