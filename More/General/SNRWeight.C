/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/SNRWeight.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

//! Default constructor
Pulsar::SNRWeight::SNRWeight ()
{
  threshold = 0.0;
}

//! Set integration weights
void Pulsar::SNRWeight::weight (Integration* integration)
{
  unsigned nchan = integration->get_nchan();

  for (unsigned ichan=0; ichan < nchan; ichan++) {

    // always ignore zapped channels
    if (integration->get_weight (ichan) == 0)
      continue;

    float snr = integration->get_Profile(0,ichan)->snr ();

    if (threshold && snr < threshold)
      snr = 0;

    integration->set_weight (ichan, snr*snr);

  }
}
