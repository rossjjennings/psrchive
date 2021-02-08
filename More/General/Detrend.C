/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Detrend.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::Detrend::Detrend ()
{
}

Pulsar::Detrend::~Detrend ()
{
}

void Pulsar::Detrend::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  vector<float> difference (nbin);

  for (unsigned i=0; i<nbin; i++)
  {
    float mean = (amps[(i+1)%nbin] + amps[(i+nbin-1)%nbin]) / 2.0;
    difference[i] = amps[i] - mean;
  }

  profile->set_amps (difference);
}
 
