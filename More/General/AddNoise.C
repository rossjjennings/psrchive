/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AddNoise.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::AddNoise::AddNoise (double sig)
{
  sigma = sig;
}

Pulsar::AddNoise::~AddNoise ()
{
}

void Pulsar::AddNoise::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();
  for (unsigned i=0; i < nbin; i++)
    amps[i] += sigma*gasdev();
}
 
