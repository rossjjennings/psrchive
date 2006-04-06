/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SquareWaveSNR.h"
#include "Pulsar/Profile.h"

//! Return the signal to noise ratio
float Pulsar::SquareWaveSNR::get_snr (const Profile* profile)
{
  int hightolow, lowtohigh, buffer;
  profile->find_transitions (hightolow, lowtohigh, buffer);

  double hi_mean, hi_var;
  profile->stats (&hi_mean, &hi_var, 0,
		  lowtohigh + buffer,
		  hightolow - buffer);

  double lo_mean, lo_var;
  profile->stats (&lo_mean, &lo_var, 0,
		  hightolow + buffer,
		  lowtohigh - buffer);

  // two unique estimates of the variance -> take the average
  double var = (lo_var + hi_var) * 0.5;

  return (hi_mean - lo_mean) / sqrt(var);
}    
