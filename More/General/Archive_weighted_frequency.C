/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/WeightedFrequency.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, rounded to nearest kHz)
  \param  ichan the index of the requested frequency channel
  \param  start the index of the first Integration to include in the mean
  \param  end one more than the index of the last Integration
*/
double Pulsar::Archive::weighted_frequency (unsigned ichan,
					    unsigned start,
					    unsigned end) const
{
  WeightedFrequency::OverEpoch weighted (this);
  weighted.set_ichan (ichan);

  return weighted (start, end);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::get_weighted_frequency
//
/*!
  \return the weighted centre frequency (in MHz, rounded to nearest kHz)
          of the whole observation (every frequency channel and Integration).
*/
double Pulsar::Archive::get_weighted_frequency() const
{
  const unsigned nsub  = get_nsubint();
  const unsigned nchan = get_nchan();
  double weighted_frequency_sum = 0.0;

  for (unsigned ichan = 0; ichan < nchan; ++ichan) {
    weighted_frequency_sum += weighted_frequency(ichan,0,nsub);
  }

  return weighted_frequency_sum / static_cast<double>(nchan);
}
