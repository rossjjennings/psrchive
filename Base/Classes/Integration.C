
#include "Integration.h"
#include "Profile.h"

/*! When this flag is true, Pulsar::Integration::invint will calculate
  the square of the polarimetric invariant interval (equal to the deteminant
  of the coherency matrix). */
bool Pulsar::Integration::default_invint_square = false;

/*! Forms the Stokes polarimetric invariant interval,
  \f$\det{P}=I^2-Q^2-U^2-V^2\f$, for every bin of each band so that,
  upon completion, npol == 1 and state == Poln::Invariant.

  \pre The baseline of each profile must have been removed [unchecked].

  \param square if true, calculate \f$ \det P=I^2-Q^2-U^2-V^2 \f$,
                otherwise calculate \f$ \sqrt{\det P} \f$

  \exception string thrown if state != Poln::Stokes
*/
void Pulsar::Integration::invint (bool square)
{
  // space to calculate the result
  vector<double> amps (nbin);

  for (int iband=0; iband<nband; ++iband) {

    int ibin;
    float* poln_amps;
    double val;

    poln_amps = (*this)[Poln::Si][iband]->get_amps ();

    for (ibin=0; ibin<nbin; ++ibin) {
      val = poln_amps[ibin];
      amps[ibin] = val * val;
    }

    for (int ipoln = 1; ipoln <= 3; ipoln++) {

      poln_amps = profiles[ipoln][iband]->get_amps();

      for (ibin=0; ibin<nbin; ++ibin) {
	val = poln_amps[ibin];
	amps[ibin] -= val * val;
      }

    }

    // prepare to reset Stokes I to the Invariant Interval
    Profile* Sinv = (*this)[Poln::Si][iband];

    // set the values
    Sinv->set_amps (amps);
    // remove the baseline
    Sinv->offset ( -Sinv->mean (Sinv->find_min_phase()) );

    if (square)
      // set the type
      Sinv->set_Poln (Poln::Sinv_sq);
    else {
      Sinv->set_Poln (Poln::Sinv);
      Sinv->square_root();
    }
    
  } // for each channel

  resize (1, nband);
  state = Poln::Invariant;
}

//! returns the dispersion delay between a frequency and reference frequency
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
  return (dm/2.41e-4)*(1.0/(freq*freq)-1.0/(reference_freq*reference_freq));
}

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

  for (int ipol=0; ipol<npol; ipol++) {
    for (int iband=0; iband<nband; iband++) {

      double cfreq = profiles[ipol][iband] -> get_centre_frequency();
      double delay = dispersion_delay (dm, frequency, cfreq);

      profiles[ipol][iband] -> rotate (delay/pfold);
    }
  }
}

/*! Computes the weighted centre frequency of an interval of sub-bands.

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
    band_end = nband;

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
  result = 0.001 * double( int(result*1000.0) );
  return result;
}
