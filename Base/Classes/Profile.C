#include <iostream>
#include <string>

#include <math.h>

#include "Pulsar/Profile.h"
#include "Physical.h"
#include "Error.h"
#include "spectra.h"
#include "f772c.h"

/*! 
  Default fractional pulse phase window used to calculate statistics
  related to the baseline.
 */
float Pulsar::Profile::default_duty_cycle = 0.15;
/*!
  Default to use new toa fit algorithm in shift
  */
bool Pulsar::Profile::legacy = false;
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
  weight = 1.0;
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
Pulsar::Profile* Pulsar::Profile::clone () const
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


vector<float> Pulsar::Profile::get_weighted_amps () const
{
  vector<float> wamps;
  
  for (unsigned i = 0; i < nbin; i++) {
    wamps.push_back(amps[i] * weight);
  }
  
  return wamps;
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

//! calculate the logarithm of each bin with value greater than threshold
void Pulsar::Profile::logarithm (double base, double threshold)
{
  if (verbose)
    cerr << "Pulsar::Profile::logarithm" << endl;
  
  float log_threshold = log(threshold)/log(base);

  // cerr << "threshold = " << log_threshold << endl;

  unsigned under = 0;

  for (unsigned ibin=0; ibin<nbin; ++ibin)
    if (amps[ibin] > threshold)
      amps[ibin] = log(amps[ibin])/log(base);
    else {
      amps[ibin] = log_threshold;
      under ++;
    }

  // cerr << "under = " << under << endl;

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
  if (verbose) {
    cerr << "Pulsar::Profile::bscrunch" << endl;
    cerr << "  Current nbin   = " << get_nbin() << endl;
    cerr << "  Scrunch factor = " << nscrunch << endl;
  }
  
  if (nscrunch < 1)
    throw Error (InvalidParam, "",
		 "nscrunch cannot be less than unity");
  
  if (nbin % nscrunch)
    throw Error (InvalidRange, "",
		 "Scrunch factor does not divide number of bins");
  
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

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::sumfabs
//
double Pulsar::Profile::sumfabs (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::sum" << endl;

  nbinify (istart, iend, nbin);

  double tot = 0;
  for (int ibin=istart; ibin < iend; ibin++)
    tot += fabs ((double) amps[ibin%nbin]);

  return tot;
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::sumsq
//
double Pulsar::Profile::sumsq (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::sumsq" << endl;
  
  nbinify (istart, iend, nbin);

  double tot = 0;
  for (int ibin=istart; ibin < iend; ibin++)  {
    double val = amps[ibin%nbin];
    tot += val * val;
  }

  return tot;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::get_ascii
//
string Pulsar::Profile::get_ascii (int bin_start, int bin_end) const
{
  if (bin_start > bin_end)
    throw Error(InvalidParam, "Pulsar::Profile::get_ascii",
		"Start bin is greater than end bin");
  
  unsigned start = 0;
  unsigned end   = get_nbin();
  
  if (bin_end > 0 && bin_end < int(get_nbin() - 1))
    end = unsigned(bin_end);
  
  if (bin_start > 0 && bin_start < int(get_nbin() - 1))
    start = unsigned(bin_start);
  
  string result;
  
  char* temp = new char[128];
  
  for (unsigned ibin=start; ibin < end; ibin++) {
    sprintf(temp, "%f", get_amps()[ibin]);
    result += temp;
    result += "\n";
  }

  delete[] temp;
  
  return result;
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

  int boxwidth = int (.5 * duty_cycle * nbin);
  if (boxwidth >= nbin/2 || boxwidth <= 0)
    throw Error (InvalidParam, "Pulsar::Profile::find_[min|max]_phase",
		 "invalid duty_cycle=%f yielded a boxwidth of %d (nbin= %d)\n",
		 duty_cycle, boxwidth*2+1, nbin);

  if (Pulsar::Profile::verbose)
    cerr << "Pulsar::Profile::find_phase duty_cycle=" << duty_cycle
	 << " boxwidth=" << boxwidth*2+1 << endl;

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
 */
float Pulsar::Profile::find_max_phase (float duty_cycle) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_max_phase" << endl;
  
  return find_phase (nbin, amps, true, duty_cycle);
}

extern "C" {
  void F772C(smooth_mw) ( float * period, int * nbin, int * maxw, float * rms,
		       int * kwmax, float * snrmax, float * smmax,
			    float * workspace);
}

float Pulsar::Profile::snr_fortran(float rms){
  int nb = get_nbin();
  int kwmax;
  float snrmax,smmax;
  float * workspace = new float[nb];
  int maxw = nb/2;
  F772C(smooth_mw)(amps,&nb,&maxw,&rms,&kwmax,&snrmax,&smmax,workspace);
  delete [] workspace;
  return(snrmax);
}

