#include "Integration.h"
#include "Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::dedisperse
//
/*!
  Rotate the profiles in each chan in order to remove the dispersion delay.
  \param frequency the frequency (in MHz) to which the delay is referenced
  \pre the period (in seconds) at which the pulsar signal was folded must
       have been previously set using Integration::set_folding_period.
  \pre the dispersion measure (in \f${\rm pc cm}^{-3}\f$) must have been
       previously set using Integration::set_dispersion_measure.
*/
void Pulsar::Integration::dedisperse (double frequency)
{
  if (dm == 0)
    return;
  if (pfold == 0)
    return;

  if (frequency == 0.0)
    frequency = weighted_frequency ();

  if (verbose)
    cerr << "Integration::dedisperse DM="<< dm <<" freq="<< frequency << endl;

  for (int ipol=0; ipol<get_npol(); ipol++)
    for (int ichan=0; ichan<get_nchan(); ichan++)
      profiles[ipol][ichan] -> dedisperse (dm, frequency, pfold);

}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, to nearest kHz)
  \retval weight the weight to be ascribed if the chan interval was integrated
  \param  chan_start the first chan included in the calculation
  \param  chan_end one more than the index of the last chan
*/
double Pulsar::Integration::weighted_frequency 
( double* weight, int chan_start, int chan_end ) const
{
  if (chan_end == 0)
    chan_end = get_nchan();

  // for now, ignore poln
  int ipol = 0;

  if (profiles.size() < 1)
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "profiles.size() == 0");

  const vector<Profile*>& prof = profiles[ipol];

  if (chan_start >= nchan || chan_start < 0)
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "chan_start=%d >= nchan=%d", chan_start, nchan);

  if (chan_end > nchan || chan_end < 0)
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "chan_end=%d > nchan=%d", chan_end, nchan);

  double weightsum = 0.0;
  double freqsum = 0.0;

  for (int ichan=chan_start; ichan < chan_end; ichan++) {
    freqsum += prof[ichan]->get_centre_frequency() * prof[ichan]->get_weight();
    weightsum += prof[ichan]->get_weight();
  }
 
  if (weight)
    *weight = weightsum;

  double result = 0.0;

  if (weightsum != 0.0)
    result = freqsum / weightsum;
  else
    result = ( prof[chan_start]->get_centre_frequency() +
	       prof[chan_end-1]->get_centre_frequency() ) / 2.0;
 
  // Nearest kHz
  result = 1e-3 * double( int(result*1e3) );
  return result;
}
