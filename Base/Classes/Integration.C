
#include "Integration.h"
#include "Profile.h"
#include "Stokes.h"

//! Returns a single Stokes 4-vector for the given band and phase bin
void Pulsar::Integration::get_Stokes ( Stokes& S, int iband, int ibin ) const
{
  if (state == Poln::Stokes) {
    for (int ipol=0; ipol<4; ++ipol)
      S[ipol] = profiles[ipol][iband]->get_amps()[ibin];
  }

  else if (state == Poln::Coherence) {

    float PP   = profiles[0][iband]->get_amps()[ibin];
    float QQ   = profiles[1][iband]->get_amps()[ibin];
    float RePQ = profiles[2][iband]->get_amps()[ibin];
    float ImPQ = profiles[3][iband]->get_amps()[ibin];

    bool circular = profiles[0][iband]->get_state() == Poln::RR;

    if (circular) {
      S.i = PP + QQ;
      S.v = PP - QQ;
      S.q = 2.0 * RePQ;
      S.u = 2.0 * ImPQ;
    }
    else {
      S.i = PP + QQ;
      S.q = PP - QQ;
      S.u = 2.0 * RePQ;
      S.v = 2.0 * ImPQ;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::get_Stokes 
//
/*!  
  The vector of Stokes parameters may be calculated with either pulse
  phase or observing frequency as the abscissa.

  \retval S a vector of Stokes parameters as a function of the given dimension
  \param iother the index in the other dimension from which to draw S
  \param abscissa the dimension of the vector abscissa

  By default, "get_Stokes (S, iband);" will return the polarimetric
  profile from band, iband.  To get the polarimetric spectra from the
  middle of the pulse profile, for example, call "get_Stokes (S,
  get_nbin()/2, Axis::Frequency);"

*/
void Pulsar::Integration::get_Stokes (vector<Stokes>& S, int iother,
				      Dimension::Axis abscissa) const
{
  int ndim = 0;
  int ndim_other = 0;

  if (!(state == Poln::Stokes || state == Poln::Coherence))
    throw string ("Pulsar::Integration::get_Stokes invalid state");

  if (abscissa == Dimension::Frequency) {
    ndim = get_nband();
    ndim_other = get_nbin();
  }
  else if (abscissa == Dimension::Phase) {
    ndim = get_nbin();
    ndim_other = get_nband();
  }
  else
    throw string ("Pulsar::Integration::get_Stokes invalid abscissa");

  if (iother<0 || iother>=ndim_other)
    throw string ("Pulsar::Integration::get_Stokes invalid dimension");

  S.resize(ndim);

  int ibin=0, iband=0;

  if (abscissa == Dimension::Frequency)
    ibin = iother;    // all Stokes values come from the same bin
  else // dim == Dimension::Phase
    iband = iother;   // all Stokes values come from the same band

  for (int idim=0; idim<ndim; idim++) {

    if (abscissa == Dimension::Frequency)
      iband = idim;
    else
      ibin = idim;

    get_Stokes (S[idim], iband, ibin);

  }
}


/*! When this flag is true, Pulsar::Integration::invint will calculate
  the square of the polarimetric invariant interval (equal to the deteminant
  of the coherency matrix). */
bool Pulsar::Integration::invint_square = false;

/*! 
  Forms the Stokes polarimetric invariant interval,
  \f$\det{P}=I^2-Q^2-U^2-V^2\f$, for every bin of each band so that,
  upon completion, npol == 1 and state == Poln::Invariant.

  If invint_square is true, this function calculates
  \f$\det\rho=I^2-Q^2-U^2-V^2\f$, otherwise \f$\sqrt{\det\rho}\f$ is
  calcuated.

  \pre The profile baselines must have been removed (unchecked).

  \exception string thrown if Stokes 4-vector cannot be formed
*/
void Pulsar::Integration::invint ()
{
  // space to calculate the result
  vector<float> invariant (get_nbin());
  // Stokes 4-vector
  Stokes stokes;

  for (int iband=0; iband<get_nband(); ++iband) {

    for (int ibin=0; ibin<get_nbin(); ++ibin) {
      // get the Stokes 4-vector
      get_Stokes (stokes, iband, ibin);
      // calculate \det\rho
      invariant[ibin] = stokes.invariant_Squared();
    }

    // prepare to reset Stokes I to the Invariant Interval
    Profile* Sinv = (*this)[Poln::Si][iband];

    // set the values
    Sinv->set_amps (invariant.begin());
    // remove the baseline
    Sinv->offset (-Sinv->mean (Sinv->find_min_phase()));

    if (invint_square)
      Sinv->set_state (Poln::Sinv_sq);
    else {
      Sinv->set_state (Poln::Sinv);
      Sinv->square_root();
    }
    
  } // for each channel

  resize (1, get_nband());
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
  if (reference_freq == 0 || freq == 0)
    throw string ("dispersion_delay: invalid frequency");
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

  for (int ipol=0; ipol<get_npol(); ipol++) {
    for (int iband=0; iband<get_nband(); iband++) {

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
  result = 0.001 * double( int(result*1000.0) );
  return result;
}
