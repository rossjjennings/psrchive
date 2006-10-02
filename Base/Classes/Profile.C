/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Profile.h"
#include "Pulsar/Config.h"
#include "Physical.h"
#include "Error.h"
#include "FTransform.h"

#include <iostream>
#include <string>

#include <math.h>

using namespace std;

/*! 
  Default fractional pulse phase window used to calculate statistics
  related to the baseline.
 */
float Pulsar::Profile::default_duty_cycle
= Pulsar::config.get<float>("baseline_duty_cycle", 0.15);

/*!  
  When true, Profile methods will output debugging information on cerr
*/
bool Pulsar::Profile::verbose
= Pulsar::config.get<bool>("Profile::verbose", false);

/*! 
  Do not allocate memory for the amps
*/
bool Pulsar::Profile::no_amps = false;


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
  amps_size = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile default constructor
//
Pulsar::Profile::Profile (unsigned nbin)
{
  init();
  resize(nbin);
  zero();
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
  nbin = _nbin;

  if (amps_size >= nbin && nbin != 0)
    return;

  if (amps) delete [] amps; amps = NULL;
  amps_size = 0;

  if (nbin == 0)
    return;

  if (!no_amps) {
    amps = new float [nbin];
    if (!amps)
      throw Error (BadAllocation, "Pulsar::Profile::resize");
    amps_size = nbin;
  }
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
  return average (&profile, 1.0);
}

const Pulsar::Profile& Pulsar::Profile::operator -= (const Profile& profile)
{
  return average (&profile, -1.0);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator +=
//
const Pulsar::Profile& Pulsar::Profile::operator += (float factor)
{
  offset (factor);
  return *this;
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator -=
//
const Pulsar::Profile& Pulsar::Profile::operator -= (float factor)
{
  offset (-factor);
  return *this;
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator *=
//
const Pulsar::Profile& Pulsar::Profile::operator *= (float factor)
{
  scale (factor);
  return *this;
}

void Pulsar::Profile::offset (double factor)
{
  for (unsigned i=0;i<nbin;i++)
    amps[i] += factor;
}

void Pulsar::Profile::scale (double factor)
{
  for (unsigned i=0;i<nbin;i++)
    amps[i] *= factor;
}

void sumdiff (Pulsar::Profile* thiz, const Pulsar::Profile* that, float factor)
{
  unsigned nbin = thiz->get_nbin();

  if (nbin != that->get_nbin())
    throw Error (InvalidParam, "Pulsar::Profile::sumdiff",
		 "nbin=%u != other nbin=%u", nbin, that->get_nbin());

  float* amps = thiz->get_amps();
  const float* pamps = that->get_amps();
  
  for (unsigned i=0;i<nbin;i++)
    amps[i] += factor * pamps[i];
}

void Pulsar::Profile::sum (const Profile* that)
{
  sumdiff (this, that, 1);
}

void Pulsar::Profile::diff (const Profile* that)
{
  sumdiff (this, that, -1);
}

vector<float> Pulsar::Profile::get_weighted_amps () const
{
  vector<float> wamps;
  
  if (weight == 0.0)
    for (unsigned i = 0; i < nbin; i++)
      wamps.push_back(0.0);
  else
    for (unsigned i = 0; i < nbin; i++)
      wamps.push_back(amps[i]);
  
  return wamps;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::zero
//
void Pulsar::Profile::zero()
{
  weight = 0;
  for (unsigned ibin = 0; ibin < nbin; ibin++)
    amps[ibin] = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::square_root
//
void Pulsar::Profile::square_root ()
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
// Pulsar::Profile::absolute
//
void Pulsar::Profile::absolute ()
{
  if (verbose)
    cerr << "Pulsar::Profile::absolute" << endl;
  
  for (unsigned ibin=0; ibin<nbin; ++ibin)
    amps[ibin] = fabs(amps[ibin]);

}

//! calculate the logarithm of each bin with value greater than threshold
void Pulsar::Profile::logarithm (double base, double threshold)
{
  if (verbose)
    cerr << "Pulsar::Profile::logarithm base=" << base 
	 << " threshold=" << threshold << endl;
  
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

