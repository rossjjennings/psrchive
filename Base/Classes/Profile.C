#include <iostream>
#include <string>

#include <math.h>

#include "Pulsar/Profile.h"
#include "Physical.h"
#include "Error.h"
#include "spectra.h"

/*! 
  Default fractional pulse phase window used to calculate statistics
  related to the baseline.
 */
float Pulsar::Profile::default_duty_cycle = 0.15;

/*!  
  When true, Profile methods will output debugging information on cerr
*/
bool Pulsar::Profile::verbose = false;

/////////////////////////////////////////////////////////////////////////////
//
// nbinify - utility for correcting the indeces of a range
//
void nbinify (int& istart, int& iend, int nbin)
{
  if (istart < 0)
    istart += ( -istart/nbin + 1 ) * nbin;

  if (iend <= istart)
    iend += ( (istart-iend)/nbin + 1 ) * nbin;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::init
//
void Pulsar::Profile::init()
{
  nbin   = 0;
  state  = Signal::None;
  weight = 0.0;
  centrefreq = -1.0;
  amps = NULL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile destructor
//

Pulsar::Profile::~Profile () 
{
  if (amps != NULL) delete [] amps;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::resize
//
void Pulsar::Profile::resize (unsigned _nbin)
{
  if (nbin == _nbin)
    return;

  if (amps != NULL) delete [] amps; amps = NULL;

  nbin = _nbin;

  if (nbin == 0)
    return;

  amps = new float [nbin];
  if (!amps)
    throw Error (BadAllocation, "Pulsar::Profile::resize");
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::clone
//
Pulsar::Profile* Pulsar::Profile::clone ()
{
  Profile* retval = new Profile (*this);
  if (!retval)
    throw Error (BadAllocation, "Pulsar::Profile::clone");
  return retval;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator =
//
/*!
  Sets all attributes of this Profile equal to that of the input Profile,
  resizes and copies the amps array.
*/
const Pulsar::Profile& Pulsar::Profile::operator = (const Profile& input)
{
  if (this == &input)
    return *this;

  try {
    resize (input.nbin);

    set_amps( input.get_amps() );
    set_weight ( input.get_weight() );
    set_centre_frequency ( input.get_centre_frequency() );
    set_state ( input.get_state() );
  }
  catch (Error& error) {
    throw error += "Pulsar::Profile::operator =";
  }

  return *this;
}

const Pulsar::Profile& Pulsar::Profile::operator += (const Profile& profile)
{
  return average (profile, 1.0);
}

const Pulsar::Profile& Pulsar::Profile::operator -= (const Profile& profile)
{
  return average (profile, -1.0);
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator +=
//
const Pulsar::Profile& Pulsar::Profile::operator += (float offset)
{
  for (unsigned i=0;i<nbin;i++)
    amps[i]+=offset;
  return *this;
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator -=
//
const Pulsar::Profile& Pulsar::Profile::operator -= (float offset)
{
  for (unsigned i=0;i<nbin;i++)
    amps[i]-=offset;
  return *this;
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator *=
//
const Pulsar::Profile& Pulsar::Profile::operator *= (float factor)
{
  for (unsigned i=0;i<nbin;i++)
    amps[i]*=factor;
  return *this;
}


void Pulsar::Profile::get_amps (float* data, unsigned jbin) const
{
  register float* dptr = data;
  register float* aptr = amps;
  register unsigned ibin;

  for (ibin=0; ibin<nbin; ibin++) {
    *dptr = *aptr;
    aptr ++; dptr += jbin;
  }
}

 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::rotate
//
/*!  
  Rotate the profile by the specified phase.  The profile will be
  rotated such that the power at phase will be found at phase zero. ie.

  \f$t^\prime=t+\phi P\f$

  where \f$t^\prime\f$ is the new start time (rising edge of bin 0),
  \f$t\f$ is the original start time, \f$\phi\f$ is equal to phase,
  and \f$P\f$ is the period at the time of folding.
*/
void Pulsar::Profile::rotate (double phase)
{
  if (verbose)
    cerr << "Pulsar::Profile::rotate phase=" << phase << " nbin=" << nbin << endl;

  if (phase == 0.0)
    return;

  if ( fft_shift (nbin, amps, phase*double(nbin)) !=0 )
    throw Error (FailedCall, "Pulsar::Profile::rotate",
		 "fft_shift(%lf) failure", phase);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::dedisperse
//
/*!
  A convenience interface to Profile::rotate.  Rotates the profile in order
  to remove the dispersion delay with respect to a reference frequency.
  \param dm the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
  \param ref_freq the reference frequency (in MHz)
  \param pfold the folding periond (in seconds)
  \post centrefreq will be set to ref_freq
*/
void Pulsar::Profile::dedisperse (double dm, double ref_freq, double pfold)
{
  if (verbose)
    cerr << "Pulsar::Profile::dedisperse dm=" << dm << " pfold=" << pfold 
	 << " ref_freq=" << ref_freq << endl;

  double delay = dispersion_delay (dm, ref_freq, centrefreq);

  if (verbose)
    cerr << "Pulsar::Profile::dedisperse delay=" << delay << " seconds" << endl;

  rotate (delay / pfold);
  set_centre_frequency (ref_freq);
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::zero
//
void Pulsar::Profile::zero()
{
  if (verbose)
    cerr << "Pulsar::Profile::zero" << endl;
  
  weight = 0;
  for (unsigned ibin = 0; ibin < nbin; ibin++)
    amps[ibin] = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::square_root
//
void Pulsar::Profile::square_root()
{
  if (verbose)
    cerr << "Pulsar::Profile::square_root" << endl;
  
  for (unsigned ibin=0; ibin<nbin; ++ibin) {
    float sign = (amps[ibin]>0) ? 1.0 : -1.0;
    amps[ibin] = sign * sqrt(float(sign * amps[ibin]));
  }
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::fold
//
void Pulsar::Profile::fold (unsigned nfold)
{
  if (verbose)
    cerr << "Pulsar::Profile::fold" << endl;
  
  if (nbin % nfold)
    throw Error (InvalidRange, "Pulsar::Profile::fold",
		 "nbin=%d %% nfold=%d != 0", nbin, nfold);
  
  unsigned newbin = nbin/nfold;

  for (unsigned i=0; i<newbin; i++)
    for (unsigned j=1; j<nfold; j++)
      amps[i] += amps[i+j*newbin];

  nbin = newbin;

  operator *= (1.0/float(nfold));
}
 

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::bscrunch
//
void Pulsar::Profile::bscrunch (unsigned nscrunch) { try
{
  if (verbose)
    cerr << "Pulsar::Profile::bscrunch" << endl;
  
  if (nscrunch < 1)
    throw Error (InvalidParam, 0, "nscrunch=%d", nscrunch);
  
  if (nbin % nscrunch)
    throw Error (InvalidRange, 0, 
		 "nbin=%d %% nscrunch=%d != 0", nbin, nscrunch);

  unsigned newbin = nbin/nscrunch;

  for (unsigned i=0; i<newbin; i++) {
    amps[i] = amps[i*nscrunch];
    for (unsigned j=1; j<nscrunch; j++)
      amps[i] += amps[i*nscrunch+j];
  }

  nbin = newbin;

  operator *= (1.0/float(nscrunch));
}
catch (Error& error) {
  throw error += "Pulsar::Profile::bscrunch";
}
} // end function

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::halvebins
//
void Pulsar::Profile::halvebins (unsigned nhalve)
{
  if (verbose)
    cerr << "Pulsar::Profile::halvebins" << endl;
  
  for (unsigned i=0; i<nhalve && nbin>1; i++) {

    if (nbin % 2)
      throw Error (InvalidRange, "Pulsar::Profile::halvebins", 
		   "nbin=%d %% 2 != 0", nbin);
    nbin /= 2;
    for (unsigned nb = 0; nb < nbin; nb++)
      amps[nb] = 0.5*(amps[2*nb] + amps[2*nb+1]);

  }
}

/////////////////////////////////////////////////////////////////////////////
//
// minmax - worker function for Pulsar::Profile::<bin_>[min|max]
//
void minmax (int nbin, const float* amps, int* mi, float* mv, bool max,
	     int istart, int iend)
{
  nbinify (istart, iend, nbin);

  float val = 0;

  float best = amps[istart%nbin];
  int ibest = istart;

  for (int ibin=istart+1; ibin < iend; ibin++) {
    val = amps[ibin%nbin];
    if ( (max && val > best) || (!max && val < best) ) {
      best = val;
      ibest = ibin;
    }
  }
  if (mi)
    *mi = ibest;
  if (mv)
    *mv = best;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_max_bin
//
int Pulsar::Profile::find_max_bin (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_max_bin" << endl;
  
  int imax=0;
  minmax (nbin, amps, &imax, 0, true, istart, iend);
  return imax;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_min_bin
//
int Pulsar::Profile::find_min_bin (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_min_bin" << endl;
  
  int imin=0;
  minmax (nbin, amps, &imin, 0, false, istart, iend);
  return imin;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::max
//
float Pulsar::Profile::max (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::max" << endl;
  
  float maxval=0;
  minmax (nbin, amps, 0, &maxval, true, istart, iend);
  return maxval;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::min
//
float Pulsar::Profile::min (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::min" << endl;
  
  float minval=0;
  minmax (nbin, amps, 0, &minval, false, istart, iend);
  return minval;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::sum
//
double Pulsar::Profile::sum (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::sum" << endl;

  nbinify (istart, iend, nbin);

  double tot = 0;
  for (int ibin=istart; ibin < iend; ibin++)
    tot += (double) amps[ibin%nbin];

  return tot;
}

#if 0
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::mean
//
double Pulsar::Profile::mean (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::mean" << endl;

  nbinify (istart, iend, nbin);
  int totbin = iend - istart;
  return sum (istart, iend) / double(totbin);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::rms
//
double Pulsar::Profile::rms (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::rms" << endl;

  nbinify (istart, iend, nbin);
  int totbin = iend - istart;

  double sumsq = 0.0;
  float amp, mean_amp = mean (istart, iend);

  for (int ibin=istart; ibin < iend; ibin++) {
    amp = amps[ibin%nbin] - mean_amp;
    sumsq += amp*amp;
  }

  return sqrt(sumsq/totbin);
}

#endif

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::stats
//
/*! Returns the mean, variance, and variance of the mean over the specified
  interval.
  \retval mean the mean of the interval
  \retval variance the variance of the interval
  \retval varmean the variance of the mean of the interval
  \param istart the first bin of the interval
  \param iend one greater than the last bin of the interval
*/
void Pulsar::Profile::stats (double* mean, double* variance, double* varmean,
			     int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::stats" << endl;
  
  unsigned counts = 0; 
  double tot = 0;
  double totsq = 0;

  if (verbose)
    cerr << "Pulsar::Profile::stats"
      " start:" << istart << 
      " stop:" << iend << endl;

  nbinify (istart, iend, nbin);

  for (int ibin=istart; ibin < iend; ibin++) {
    double value = amps[ibin%nbin];
    tot += value;
    totsq += value*value;
    counts ++;
  }

  if (counts<2)
    throw Error (InvalidRange, "Pulsar::Profile::stats",
		 "%d -> %d", istart, iend);

  //
  // variance(x) = <(x-<x>)^2> * N/(N-1) = (<x^2>-<x>^2) * N/(N-1)
  //
  double mean_x   = tot / double(counts);
  double mean_xsq = totsq / double(counts);
  double var_x = (mean_xsq - mean_x*mean_x) * double(counts)/double(counts-1);

  if (mean)
    *mean = mean_x;
  if (variance)
    *variance = var_x;
  if (varmean)
    *varmean = var_x / double(counts);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::stats
//
/*! 
  \param phase centre of region
  \retval mean the mean of the interval
  \retval variance the variance of the interval
  \retval varmean the variance of the mean of the interval
  \param istart the first bin of the interval
  \param iend one greater than the last bin of the interval
  \param duty_cycle width of region
  */
void Pulsar::Profile::stats (float phase,
			     double* mean, 
			     double* variance,
			     double* varmean,
			     float duty_cycle) const
{
  if (verbose)
    cerr << "Pulsar::Profile::stats" << endl;

  int start_bin = int ((phase - 0.5 * duty_cycle) * nbin);
  int stop_bin = int ((phase + 0.5 * duty_cycle) * nbin);

  stats (mean, variance, varmean, start_bin, stop_bin);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::mean
//
/*! 
  \param phase centre of region
  \param duty_cycle width of region
  \return mean of region
*/
double Pulsar::Profile::mean (float phase, float duty_cycle) const
{
  double result;
  stats (phase, &result, 0, 0, duty_cycle);
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
// find_phase - worker function for Profile::find_[min|max]_phase
//
float find_phase (int nbin, float* amps, bool max, float duty_cycle)
{
  register int i, j;

  int boxwidth = (int) (.5 * duty_cycle * nbin);
  if (boxwidth >= nbin/2 || boxwidth <= 0)
    throw Error (InvalidParam, "Pulsar::Profile::find_[min|max]_phase",
		 " invalid duty_cycle=%f\n", duty_cycle);

  double sum = 0.0;
  for (j=-boxwidth;j<=boxwidth;j++)
    sum+=amps[(j+nbin) % nbin];

  int bin = 0;
  double val = sum;
  for (i=1;i<nbin;i++) {
    sum = sum + amps[(i+boxwidth+nbin)%nbin] 
             - amps[(i-boxwidth-1+nbin)%nbin];
    if ( (max && sum > val) || (!max && sum < val) ) {
      val=sum;
      bin = i;
    }
  }

  return float(bin) / float(nbin);
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_min_phase
//
/*! Returns the centre phase of the region with minimum mean
  \param duty_cycle width of the region over which the mean is calculated
  \retval min_val returns the minimum mean
 */
float Pulsar::Profile::find_min_phase (float duty_cycle) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_min_phase" << endl;
  
  return find_phase (nbin, amps, false, duty_cycle);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_max_phase
//
/*! Returns the centre phase of the region with maximum mean
  \param duty_cycle width of the region over which the mean is calculated
  \retval max_val returns the maximum mean
 */
float Pulsar::Profile::find_max_phase (float duty_cycle) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_max_phase" << endl;
  
  return find_phase (nbin, amps, true, duty_cycle);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::flux
//
/*!
  Using Profile::find_min_phase and Profile::find_peak_edges, this
  function finds the integrated flux in the pulse profile
*/
float Pulsar::Profile::flux() const
{
  if (verbose)
    cerr << "Pulsar::Profile::flux" << endl;
  
  // find the mean of the baseline
  double min_avg, min_var;
  stats (find_min_phase(), &min_avg, &min_var);

  if (verbose)
    cerr << "Pulsar::Profile::flux mean(off pulse) =" << min_avg << endl;

  // find the total power under the pulse
  int rise = 0, fall = 0;
  find_peak_edges (rise, fall);

  double flux = sum (rise, fall);
  
  // divide by the number of bins
  flux /=  (double(fall-rise));
  
  // subtract the total power due to the baseline
  flux -= min_avg ;


  // multiply by onpulse fraction
  flux *= (double (fall-rise))/(get_nbin());

  return flux;
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::snr
//
/*!
  Using Profile::find_min_phase and Profile::find_peak_edges, this
  function finds the integrated power in the pulse profile and divides
  this by the noise in the baseline.
*/
float Pulsar::Profile::snr() const
{
  if (verbose)
    cerr << "Pulsar::Profile::snr" << endl;
  
  // find the mean and the r.m.s. of the baseline
  double min_avg, min_var;
  stats (find_min_phase(), &min_avg, &min_var);
  double min_rms = sqrt (min_var);

  if (verbose)
    cerr << "Pulsar::Profile::snr rms=" << min_rms << endl;

  if (min_rms == 0.0)
    return 0;

  // find the total power under the pulse
  int rise = 0, fall = 0;
  find_peak_edges (rise, fall);

  double power = sum (rise, fall);

  // subtract the total power due to the baseline
  power -= min_avg * double (fall - rise);

  // divide by the sqrt of the number of bins
  power /= sqrt (double(fall-rise));

  return power/min_rms;
}


float Pulsar::Profile::snr (const Profile& std) const
{
  if (verbose)
    cerr << "Pulsar::Profile::snr(const Profile& std)" << endl;

  float ephase, snrfft, esnrfft;

  try {
    shift (std, ephase, snrfft, esnrfft);
  }
  catch (...) {
    return 0.0;
  }

  return snrfft;
}

#if defined(sun) && !defined(__GNUC__)

void Pulsar::Profile::set_amps (const float* data)
{
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps[ibin] = data[ibin];
}

#endif


