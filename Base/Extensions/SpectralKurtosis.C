/***************************************************************************
 *
 *   Copyright (C) 2011 by Andrew Jameson and Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SpectralKurtosis.h"

using namespace std;

Pulsar::SpectralKurtosis::SpectralKurtosis () : Extension ("SpectralKurtosis")
{
  M = 0;
  nsigma = 0;
  nchan = 0;
  npol = 0;
  loader = 0;
  ipol_mean = 0;

}

Pulsar::SpectralKurtosis::SpectralKurtosis (const SpectralKurtosis& extension)
  : Extension ("SpectralKurtosis")
{
  operator = (extension);
}

const Pulsar::SpectralKurtosis&
Pulsar::SpectralKurtosis::operator= (const SpectralKurtosis& sk)
{
  M = sk.M;
  nsigma = sk.nsigma;
  nchan = sk.nchan;
  npol = sk.npol;
  filtered_sum = sk.filtered_sum;
  filtered_hits = sk.filtered_hits;
  unfiltered_sum = sk.unfiltered_sum;
  unfiltered_hits = sk.unfiltered_hits;

  return *this;
}

const Pulsar::SpectralKurtosis&
Pulsar::SpectralKurtosis::operator += (const SpectralKurtosis& sk)
{
  if (M != sk.M || nsigma != sk.nsigma || nchan != sk.nchan || npol != sk.npol)
  {
    M = 0;
    nsigma = 0;
    nchan = 0;
    npol = 0;
    resize(0, 0);
    if (Integration::verbose)
      cerr << "Pulsar::SpectralKurtosis::+= param mismatch, discarding SpectralKurtosis "
           << "statistics" << endl;
  }
  else
  {
    // sum the sums and hits for each channel and polarization
    for (unsigned i=0; i<npol*nchan; i++)
    {
      filtered_sum[i]   += sk.filtered_sum[i];
      filtered_hits[i]  += sk.filtered_hits[i];
      unfiltered_sum[i] += sk.unfiltered_sum[i];
    }
    unfiltered_hits += unfiltered_hits;
  }

  return *this;
}

Pulsar::SpectralKurtosis::~SpectralKurtosis ()
{
}

void Pulsar::SpectralKurtosis::set_M (unsigned _M)
{
  M = _M;
}

unsigned Pulsar::SpectralKurtosis::get_M () const
{
  get_data();
  return M;
}

void Pulsar::SpectralKurtosis::set_excision_threshold (unsigned _nsigma)
{
  nsigma = _nsigma;
}

unsigned Pulsar::SpectralKurtosis::get_excision_threshold () const
{
  get_data();
  return nsigma;
}

float Pulsar::SpectralKurtosis::get_filtered_sum (unsigned ichan, unsigned ipol) const
{
  get_data();
  range_check (ichan, ipol, "SpectralKurtosis::get_filtered_sum");
  return filtered_sum [nchan*ipol + ichan];
}

void Pulsar::SpectralKurtosis::set_filtered_sum (unsigned ichan, unsigned ipol, float sum)
{
  range_check (ichan, ipol, "SpectralKurtosis::set_filtered_sum");

  filtered_sum.resize (nchan * npol);
  filtered_sum [nchan*ipol + ichan] = sum;
}

uint64_t Pulsar::SpectralKurtosis::get_filtered_hits (unsigned ichan) const
{
  get_data();
  range_check (ichan, 0, "SpectralKurtosis::get_filtered_hits");
  return filtered_hits [ichan];
}

void Pulsar::SpectralKurtosis::set_filtered_hits (unsigned ichan, uint64_t hits)
{
  range_check (ichan, 0, "SpectralKurtosis::set_filtered_hits");
  filtered_hits.resize (nchan);
  filtered_hits [ichan] = hits;
}

float Pulsar::SpectralKurtosis::get_unfiltered_sum (unsigned ichan, unsigned ipol) const
{
  get_data();
  range_check (ichan, ipol, "SpectralKurtosis::get_unfiltered_sum");
  return unfiltered_sum [nchan*ipol + ichan];
}

void Pulsar::SpectralKurtosis::set_unfiltered_sum (unsigned ichan, unsigned ipol, float sum)
{
  range_check (ichan, ipol, "SpectralKurtosis::set_unfiltered_sum");
  unfiltered_sum.resize (nchan * npol);
  unfiltered_sum [nchan*ipol + ichan] = sum;
}

uint64_t Pulsar::SpectralKurtosis::get_unfiltered_hits () const
{
  get_data();
  return unfiltered_hits;
}

void Pulsar::SpectralKurtosis::set_unfiltered_hits (uint64_t hits)
{
  unfiltered_hits = hits;
}

//! Get the unfiltered mean of the specified channel
float Pulsar::SpectralKurtosis::get_unfiltered_mean (unsigned ichan) const
{
  return get_unfiltered_sum(ichan, ipol_mean) / get_unfiltered_hits();
}

float Pulsar::SpectralKurtosis::get_filtered_mean (unsigned ichan) const
{
  if (get_filtered_hits(ichan))
    return get_filtered_sum(ichan, ipol_mean) / get_filtered_hits(ichan);
  else
    return 0;
}

void Pulsar::SpectralKurtosis::range_check (unsigned ichan, unsigned ipol, 
                                            const char* method) const
{
  if (ichan >= nchan)
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d", ichan, nchan);
  if (ipol >= npol)
    throw Error (InvalidRange, method, "ipol=%d >= npol=%d", ipol, npol);
}

void Pulsar::SpectralKurtosis::set_loader (SpectralKurtosis::Loader * _loader)
{
  loader = _loader;
}

/*! Resize internal storage for npol and nchan */  
void Pulsar::SpectralKurtosis::resize(unsigned _npol, unsigned _nchan)
{
  npol = _npol;
  nchan = _nchan;
  filtered_sum.resize(npol*nchan);
  filtered_hits.resize(npol*nchan);
  unfiltered_sum.resize(npol*nchan);
}

/*! Combine SK statistics from another integration. */
void Pulsar::SpectralKurtosis::integrate (const Integration* subint)
{ 
  const SpectralKurtosis* useful = subint->get<SpectralKurtosis>();

  if (!useful) {
    if (Integration::verbose)
      cerr << "Pulsar::SpectralKurtosis::integrate subint has no SpectralKurtosis" << endl;
    return;
  }

  if (Integration::verbose)
    cerr << "Pulsar::SpectralKurtosis::integrate other SpectralKurtosis" << endl;

  operator += (*useful);
}

/*! Update parameters from supplied subint */
void Pulsar::SpectralKurtosis::update (const Integration* subint)
{
  const SpectralKurtosis* useful = subint->get<SpectralKurtosis>();

  if (!useful) {
    if (Integration::verbose)
      cerr << "Pulsar::SpectralKurtosis::update subint has no SpectralKurtosis" << endl;
    return;
  }

  if (Integration::verbose)
    cerr << "Pulsar::SpectralKurtosis::update other SpectralKurtosis" << endl;

  M = useful->M;
  nsigma = useful->nsigma;
  resize(useful->npol, useful->nchan);
  return;
}

/*! Load the SK data from file via the loader, but only do this once */
void Pulsar::SpectralKurtosis::get_data () const
{
  if (loader)
    const_cast<SpectralKurtosis *>(this)->load();
}

void Pulsar::SpectralKurtosis::load ()
{
  if (Integration::verbose)
    cerr << "Pulsar::SpectralKurtosis::load" << endl;
  loader->load (this);
  loader = 0;
}

/*! Return a text interfaces that can be used to access this instance */
TextInterface::Parser* Pulsar::SpectralKurtosis::get_interface()
{
  return new Interface( this );
}

