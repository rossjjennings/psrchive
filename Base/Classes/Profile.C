#include <math.h>

#include <iostream>
#include <string>

#include "Profile.h"

#include "spectra.h"
#include "genutil.h"

extern "C" {
 void shiftbyfft_(float *, int *, float *);
}


/*! 
  fractional pulse phase window used by default to calculate the
  the minimum or maximum of the pulse profile 
 */
float Pulsar::Profile::default_duty_cycle = 0.15;

/*! 
  for debugging.  Add the statement "Pulsar::Profile::verbose =
  true;" in the calling program to have debug information output on stderr 
*/
bool Pulsar::Profile::verbose = 0;

// utility for correcting the indeces for a range
void nbinify (int& istart, int& iend, int nbin)
{
  if (istart < 0)
    istart += ( -istart/nbin + 1 ) * nbin;

  if (iend <= istart)
    iend += ( (istart-iend)/nbin + 1 ) * nbin;
}

Pulsar::Profile::Profile()
{
  nbin   = 0;
  state  = Poln::None;
  weight = 0.0;
  centrefreq = -1.0;
  amps = NULL;
}

Pulsar::Profile::~Profile()
{
  if (amps != NULL) delete [] amps;  amps = NULL;
}

void Pulsar::Profile::offset (float offset)
{
  for (int i=0;i<nbin;i++)
    amps[i]+=offset;
}
 
void Pulsar::Profile::scale (float factor)
{
  for (int i=0;i<nbin;i++)
    amps[i]*=factor;
}
 
/*!  Rotate the profile by the specified phase.  The profile will be
 rotated such that the power at phase will be found at phase zero.
 ie. \f$t^\prime=t+\phi P\f$, where \f$t^\prime\f$ is the new start
 time (rising edge of bin 0), \f$t\f$ is the original start time,
 \f$\phi\f$ is equal to phase, and \f$P\f$ is the period at the time
 of folding.  */
void Pulsar::Profile::rotate (double phase)
{
  if ( fft_shift (nbin, amps, phase*double(nbin)) !=0 )
    throw_str ("Pulsar::Profile::rotate (%lf) failure", phase);
}


void Pulsar::Profile::zero()
{
  weight = 0;
  for (int ibin = 0; ibin < nbin; ibin++)
    amps[ibin] = 0;
}
 
void Pulsar::Profile::square_root()
{
  for (int ibin=0; ibin<nbin; ++ibin) {
    float sign = (amps[ibin]>0) ? 1.0 : -1.0;
    amps[ibin] = sign * sqrt(sign * amps[ibin]);
  }
}


void Pulsar::Profile::fold (int nfold)
{
  for (int i=0; i<nbin/nfold;i++)
    for (int j=1; j<nfold; j++)
       amps[i]+=amps[i+j*nbin/nfold];
  nbin/=nfold;
  scale (1.0/float(nfold));
}
 
void Pulsar::Profile::halvebins (int nhalve)
{
  /* only need to do if nhalve > 0 */
  if(nhalve>0) {
    for (int i=0;i<nhalve;i++) {
      if(nbin>1) {
        for (int nb = 0; nb < nbin/2; nb++)
           amps[nb] = 0.5*(amps[2*nb] + amps[2*nb+1]);
        nbin = nbin/2;
      }
    }
  }
}
 
int Pulsar::Profile::bin_max () const
{
  float highest = amps[0];
  int imax = 0;

  for (int ibin =0; ibin<nbin; ibin++)
    if (amps[ibin] > highest) {
      highest = amps[ibin];
      imax = ibin;
    }

  return imax;
}
 
int Pulsar::Profile::bin_min () const
{
  float lowest = amps[0];
  int imin = 0;

  for (int ibin =0; ibin<nbin; ibin++)
    if (amps[ibin] < lowest) {
      lowest = amps[ibin];
      imin = ibin;
    }

  return imin;
}
 
float Pulsar::Profile::max (int istart, int iend) const
{
  nbinify (istart, iend, nbin);

  float val = 0;
  float highest = amps[istart%nbin];
  for (int ibin=istart; ibin < iend; ibin++) {
    val = amps[ibin%nbin];
    if (val > highest)
      highest = val;
  }
  return highest;
}

float Pulsar::Profile::min (int istart, int iend) const
{
  nbinify (istart, iend, nbin);

  float val = 0;
  float lowest = amps[istart%nbin];
  for (int ibin=istart; ibin < iend; ibin++) {
    val = amps[ibin%nbin];
    if (val < lowest)
      lowest = val;
  }

  return lowest;
}

double Pulsar::Profile::sum (int istart, int iend) const
{
  nbinify (istart, iend, nbin);

  double tot = 0;
  for (int ibin=istart; ibin < iend; ibin++)
    tot += (double) amps[ibin%nbin];

  return tot;
}

double Pulsar::Profile::mean (int istart, int iend) const
{
  nbinify (istart, iend, nbin);
  int totbin = iend - istart;
  return sum (istart, iend) / double(totbin);
}

double Pulsar::Profile::rms (int istart, int iend) const
{
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

double Pulsar::Profile::mean (float phase, float duty_cycle,
			      double* varmean) const
{
  int start_bin = int ((phase - 0.5 * duty_cycle) * nbin);
  int stop_bin = int ((phase + 0.5 * duty_cycle) * nbin);

  double meanval;
  stats (&meanval, 0, varmean, start_bin, stop_bin);
  return meanval;
}

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
    throw_str ("Pulsar::Profile::stats ERROR invalid range: %d -> %d",
	       istart, iend);

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

//
// worker function for the following two methods
//
float find_phase (int nbin, float* amps, bool max, float duty_cycle)
{
  register int i, j;

  int boxwidth = (int) (.5 * duty_cycle * nbin);
  if (boxwidth >= nbin/2)
    throw_str ("Pulsar::Profile::find_[min|max]_phase ERROR"
	       " smoothing over too many points\n"
	       " nbin=%d boxwidth=%d\n", nbin, 2*boxwidth);

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
 
/*! Returns the centre phase of the region with minimum mean
  \param duty_cycle width of the region over which the mean is calculated
  \retval min_val returns the minimum mean
 */
float Pulsar::Profile::find_min_phase (float duty_cycle) const
{
  return find_phase (nbin, amps, false, duty_cycle);
}

/*! Returns the centre phase of the region with maximum mean
  \param duty_cycle width of the region over which the mean is calculated
  \retval max_val returns the maximum mean
 */
float Pulsar::Profile::find_max_phase (float duty_cycle) const
{
  return find_phase (nbin, amps, true, duty_cycle);
}


//! Returns the r.m.s. at the 
double Pulsar::Profile::sigma (float phase, float duty_cycle) const
{
  int start_bin = int ((phase - 0.5 * duty_cycle) * nbin);
  int stop_bin = int ((phase + 0.5 * duty_cycle) * nbin);

  return rms (start_bin, stop_bin);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::snr
//
/*!  
  Using Profile::find_peak_edges(), this function finds 
 */
float Pulsar::Profile::snr() const
{
  // find the mean and the r.m.s. of the baseline
  float min_ph = find_min_phase ();
  double min_avg = mean (min_ph);
  double min_rms = sigma (min_ph);

  // find the total power under the pulse
  int rise = 0, fall = 0;
  find_peak_edges (rise, fall);

  double power = sum (rise, fall);

  // subtract the total power due to the baseline
  power -= min_avg * double (fall - rise);

  // divide by the sqrt of the number of bins
  power /= sqrt (fall-rise);

  return power/min_rms;
}

#if 0
/**************************************************************/
/* An adaption of the snr funtion above                       */
/* Instead of calculating the rms using the sigma() function  */
/* we pass the function an rms calculated from the residuals  */
/* of fitting the profile to the standard                     */
/**************************************************************/
float Pulsar::Profile::std_snr (float noise, float wing_sigma,
		       int& bin_st, int& bin_end) const
{
  if (verbose)
    cerr << "Pulsar::Profile::std_snr enter" << endl;

  if (noise <= 0.0)
    return 0.0;

  // Form cumulative sum
  double * cumu = new double[nbin*2];
  int ibin;

  cumu[0] = amps[0];
  for (ibin=1; ibin<2*nbin; ibin++)
    cumu[ibin] = cumu[ibin-1] + amps[ibin%nbin];
  
  // work out where cumulative sum falls below 10% of sum for the
  // last time, and where it drops below 90% for the first time.
  
  int iten = 0;
  int ininety = nbin-1;

  double limit = wing_sigma * cumu[nbin-1];
  for (ibin=0; ibin<nbin; ibin++) if (cumu[ibin] < limit) iten = ibin;

  limit = (1.0-wing_sigma) * cumu[nbin-1];
  for (ibin=nbin-1; ibin>=0; ibin--) if (cumu[ibin] > limit) ininety = ibin;
  
  bin_end = ininety;
  bin_st = iten;

  // work out where cumulative sum falls below 10% of sum for the
  // last time, and where it drops below 90% for the first time this
  // time starting half way along.
  
  int iten2, ininety2;
  int izero = iten2 = nbin/2;
  int ifull = ininety2 = nbin-1+nbin/2;
  limit = wing_sigma * (cumu[ifull]-cumu[izero]);
  for (ibin=izero; ibin<ifull; ibin++)
    if (cumu[ibin]-cumu[izero] < limit) iten2 = ibin;

  limit = (1.0-wing_sigma) * (cumu[ifull]-cumu[izero]);
  for (ibin=ifull; ibin>=izero; ibin--)
    if (cumu[ibin]-cumu[izero] > limit) ininety2 = ibin;
  //  printf("Signal to noise ratio is %f   duty cycle is %5.2f \%\n",
  //       (cumu[ininety2]-cumu[iten2])/sqrt(ininety2-iten2+1),
  //        (float) (ininety2-iten2)/nbin * 100.0);
  // Decide which one is better - ie narrower.

  if (ininety2-iten2 < ininety-iten) {
    ininety = ininety2;
    iten = iten2;
    bin_end = ininety - nbin/2;
    bin_st = iten - nbin/2;
  }

  //cerr << "Pulsar::Profile::std_snr i10=" << iten << " i90=" << ininety << endl;
  float signal = (cumu[ininety]-cumu[iten])/sqrt(ininety-iten+1);
  delete [] cumu;
  
  if (verbose)
    cerr << "Pulsar::Profile::std_snr exit" << endl;

  return signal / noise;
}


/*****************************************************************************/
/* This funtion is very much like fittempl but returns the snr of a profile  */
/*****************************************************************************/
float Pulsar::Profile::snr (const profile& std, float& noise, bool allow_rotate) const
{
  if (verbose)
    cerr << "Pulsar::Profile::snr " << nbin << " bins." << endl;

  if (std.nbin != nbin)
    throw string ("Pulsar::Profile::snr std with unequal nbin");

  profile cpy = *this;

  if (allow_rotate) {
    try {
      float junk1,junk2,junk3;
      double shift = this->shift(std,&junk1,&junk2,&junk3);
      if (shift == -2)
	return -1.0;
      if (verbose)
	cerr << "Pulsar::Profile::snr rotate profile by " << shift << endl;
      cpy.rotate (shift); 
    }
    catch (...) {
      return 0.0;
    }
  }

  // calculate the scale and offset where:
  // cpy = offset + scale * std
  // see derivation in Willem's thesis appendix, or do it yourself
  double s_sum=0, ss_sum=0, p_sum=0, ps_sum=0;
  int ibin;
  for (ibin=0; ibin<nbin; ibin++) {
    double s = std.amps[ibin];
    double p = cpy.amps[ibin];
    s_sum += s;
    ss_sum += s * s;
    p_sum += p;
    ps_sum += p * s;
  }
  double s_avg = s_sum / nbin;
  double scale  = (ps_sum - p_sum*s_avg) / (ss_sum - s_sum*s_avg);
  double offset = (p_sum - scale * s_sum) / nbin;

  if (verbose)
    cerr << "Pulsar::Profile::snr scale:" << scale << " offset:" << offset << endl;

  // determine the strength of the signal in the standard,
  // (as returned when noise=1) and scale appropriately
  float signal = std.std_snr (1.0, 0.10);
  if (verbose)
    cerr << "Pulsar::Profile::snr std signal " << signal << endl;

  // calculate the residual (difference) profile
  for (ibin=0; ibin < nbin; ibin++) {
    cpy.amps[ibin] -= offset + scale * std.amps[ibin];
    //    cout << ibin << " " << cpy.amps[ibin] << endl;
  }

  // determine RMS (noise) of residual profile 
  noise = cpy.rms();
  if (verbose)
    cerr << "Pulsar::Profile::snr residual noise " << noise << endl;

  return scale * signal / noise;
}


#endif

