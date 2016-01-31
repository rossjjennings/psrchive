/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/WeightedFrequency.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, rounded to nearest kHz)
  \param  chan_start the first chan included in the calculation
  \param  chan_end one more than the index of the last chan
*/
double Pulsar::Integration::weighted_frequency (unsigned chan_start,
						unsigned chan_end) const
{
  WeightedFrequency::OverFrequency weighted (this);
  return weighted (chan_start, chan_end);
}

double Pulsar::Integration::effective_bandwidth (unsigned chan_start,
						 unsigned chan_end) const
{
  unsigned nchan = get_nchan ();

  if (nchan == 0)
    throw Error (InvalidRange, "Integration::effective_bandwidth",
                 "nchan == 0");

  if (chan_end == 0)
    chan_end = nchan;

  if (chan_start >= nchan)
    throw Error (InvalidRange, "Integration::effective_bandwidth",
		 "chan_start=%d nchan=%d", chan_start, nchan);

  if (chan_end > nchan)
    throw Error (InvalidRange, "Integration::effective_bandwidth",
		 "chan_end=%d nchan=%d", chan_end, nchan);

  double sum = 0.0;
  double sumsq = 0.0;


  for (unsigned index=chan_start; index < chan_end; index++)
  {
    double weight = get_weight (index);
    
    sum += weight;
    sumsq += weight * weight;
  }
  
  if (sumsq == 0)
    return 0;

  return sum*sum/sumsq * get_bandwidth () / nchan;
}
