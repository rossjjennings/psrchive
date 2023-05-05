/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Logarithm.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::Logarithm::Logarithm ()
{
}

Pulsar::Logarithm::~Logarithm ()
{
}

void Pulsar::Logarithm::transform (Profile* profile)
{
  profile->logarithm ();
}
 
