#include "Integration.h"
#include "Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::dedisperse
//
/*!
  Rotate the profiles in each band in order to remove the dispersion delay.
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
    for (int iband=0; iband<get_nband(); iband++)
      profiles[ipol][iband] -> dedisperse (dm, frequency, pfold);

}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, to nearest kHz)
  \retval weight the weight to be ascribed if the band interval was integrated
  \param  band_start the first band included in the calculation
  \param  band_end one more than the index of the last band
*/
double Pulsar::Integration::weighted_frequency 
( double* weight, int band_start, int band_end ) const
{
  if (band_end == 0)
    band_end = get_nband();

  // for now, ignore poln
  int ipol = 0;

  if (profiles.size() < 1)
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "profiles.size() == 0");

  const vector<Profile*>& prof = profiles[ipol];

  if (band_start >= nband || band_start < 0)
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "band_start=%d >= nband=%d", band_start, nband);

  if (band_end > nband || band_end < 0)
    throw Error (InvalidRange, "Integration::weighted_frequency",
		 "band_end=%d > nband=%d", band_end, nband);

  double weightsum = 0.0;
  double freqsum = 0.0;

  for (int iband=band_start; iband < band_end; iband++) {
    freqsum += prof[iband]->get_centre_frequency()*prof[iband]->get_weight();
    weightsum += prof[iband]->get_weight();
  }
 
  if (weight)
    *weight = weightsum;

  double result = 0.0;

  if (weightsum != 0.0)
    result = freqsum / weightsum;
  else
    result = ( prof[band_start]->get_centre_frequency() +
	       prof[band_end-1]->get_centre_frequency() ) / 2.0;
 
  // Nearest kHz
  result = 1e-3 * double( int(result*1e3) );
  return result;
}
