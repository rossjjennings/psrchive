/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, to nearest kHz)
  \param  chan_start the first chan included in the calculation
  \param  chan_end one more than the index of the last chan
*/
double Pulsar::Integration::weighted_frequency (unsigned chan_start,
						unsigned chan_end) const
{
  if (verbose) cerr << "Pulsar::Integration::weighted_frequency chan start="
                    << chan_start << " end=" << chan_end << endl;

  if (chan_end == 0)
    chan_end = get_nchan();

  // for now, ignore poln
  unsigned ipol = 0;

  if (get_npol() == 0)
    throw Error (InvalidRange, "Integration::weighted_frequency", "npol==0");

  const vector< Reference::To<Profile> >& prof = profiles[ipol];

  if (chan_start >= get_nchan())
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "chan_start=%d nchan=%d", chan_start, get_nchan());

  if (chan_end > get_nchan())
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "chan_end=%d nchan=%d", chan_end, get_nchan());

  double weightsum = 0.0;
  double freqsum = 0.0;

  for (unsigned ichan=chan_start; ichan < chan_end; ichan++) {
    freqsum += prof[ichan]->get_centre_frequency() * prof[ichan]->get_weight();
    weightsum += prof[ichan]->get_weight();
  }
 
  double result = 0.0;

  if (weightsum != 0.0)
    result = freqsum / weightsum;
  else
    result = 0.5 * ( prof[chan_start]->get_centre_frequency() +
	             prof[chan_end-1]->get_centre_frequency() );

  if (verbose)
    cerr << "Integration::weighted_frequency result=" << result
         << " weight=" << weightsum << endl;
 
  // Nearest kHz
  result = 1e-3 * double( int(result*1e3) );

  if (verbose)
    cerr << "Integration::weighted_frequency rounded=" << result << endl;

  return result;
}
