#include "Integration.h"
#include "Profile.h"


//! returns the dispersion delay between a frequency and reference frequency
double dispersion_delay (double dm, double reference_freq, double freq);

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::dedisperse
//
/*!
  Rotate the profiles in each band in order to remove the dispersion delay.
  \param pfold the period (in seconds) at which the pulsar signal was folded.
  \param dm the dispersion measure in \f$ {\rm pc cm}^3 \f$
  \param frequency the frequency (in MHz) to which the delay is referenced
*/
void Pulsar::Integration::dedisperse
( double pfold, double dm, double frequency )
{
  if (frequency == -1.0)
    frequency = weighted_frequency ();

  if (verbose)
    cerr << "Integration::dedisperse DM="<< dm <<" freq="<< frequency << endl;

  for (int ipol=0; ipol<get_npol(); ipol++) {
    for (int iband=0; iband<get_nband(); iband++) {

      double cfreq = profiles[ipol][iband] -> get_centre_frequency();
      double delay = dispersion_delay (dm, frequency, cfreq);

      profiles[ipol][iband] -> rotate (delay/pfold);
    }
  }
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
  \param  poln the polarization measure on which to operate
*/
double Pulsar::Integration::weighted_frequency 
( double* weight, Poln::Measure poln, int band_start, int band_end ) const
{
  if (band_end == 0)
    band_end = get_nband();

  // for now, ignore poln
  int ipol = 0;

  if (profiles.size() < 1)
    throw string ("Integration::weighted_frequency invalid dimensions");

  const vector<Profile*>& prof = profiles[ipol];

  if (band_start > int(prof.size()) || band_start < 0)
    throw string ("Integration::weighted_frequency invalid band_start");

  if (band_end > int(prof.size()) || band_end < 0)
    throw string ("Integration::weighted_frequency invalid band_end");

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

/*! If the frequency is lower than the reference frequency, then the delay
  is positive.
  \return dispersion delay in seconds
  \param dm the dispersion measure in \f$ {\rm pc cm}^3 \f$
  \param reference_freq the frequency (in MHz) to which the delay is
         referenced
  \param freq the frequency (in MHz) of the delayed band
*/
double dispersion_delay (double dm, double reference_freq, double freq)
{
  if (reference_freq == 0 || freq == 0)
    throw string ("dispersion_delay: invalid frequency");
  return (dm/2.41e-4)*(1.0/(freq*freq)-1.0/(reference_freq*reference_freq));
}
