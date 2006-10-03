/***************************************************************************
 *
 *   Copyright (C) 2005 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/denoise.h"
#include "Pulsar/Profile.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::zap_periodic_spikes
/*! Interpolate over peaks of 1-bin wide modulation feature.

  period and phase are both measured in bins 
*/
void zap_periodic_spikes(Pulsar::Profile* profile, int period, int phase)
{
  int i, iprev, inext;
  int nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  for (i=phase; i < nbin; i+=period)
  {
    iprev = (i > 0 ? i-1 : nbin);
    inext = (i < nbin-1 ? i+1 : 0);
    amps[i] = 0.5*(amps[iprev]+amps[inext]);
  }
}
