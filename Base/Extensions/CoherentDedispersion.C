/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CoherentDedispersion.h"

using namespace Pulsar;
using namespace std;

CoherentDedispersion::CoherentDedispersion ()
  : Extension ("CoherentDedisperion")
{
  domain = Signal::Frequency;

  dispersion_measure = 0;
  doppler_correction = 0;

  nchan_input = 0;
  nchan_output = 0;

  nlev_data = 0;
  nlev_chirp = 0;
}

//! Clone method
CoherentDedispersion* CoherentDedispersion::clone () const
{
  return new CoherentDedispersion (*this);
}

//! Number of complex time samples in each cyclical convolution
unsigned CoherentDedispersion::get_nsamp (unsigned i) const
{
  check_index_out (i, "get_nsamp");
  return nsamp[i];
}

//! Number of complex time samples in each cyclical convolution
void CoherentDedispersion::set_nsamp (unsigned i, unsigned val)
{
  check_index_out (i, "set_nsamp");
  nsamp[i] = val;
}

//! Number of complex time samples in wrap-around region, left-hand side
unsigned CoherentDedispersion::get_nsamp_overlap_pos (unsigned i) const
{
  check_index_out (i, "get_nsamp_overlap_pos");
  return nsamp_overlap_pos[i];
}

//! Number of complex time samples in wrap-around region, left-hand side
void CoherentDedispersion::set_nsamp_overlap_pos (unsigned i, unsigned n)
{
  check_index_out (i, "set_nsamp_overlap_pos");
  nsamp_overlap_pos[i] = n;
}

//! Number of complex time samples in wrap-around region, right-hand side
unsigned CoherentDedispersion::get_nsamp_overlap_neg (unsigned i) const
{
  check_index_out (i, "get_nsamp_overlap_neg");
  return nsamp_overlap_neg[i];
}

//! Number of complex time samples in wrap-around region, right-hand side
void CoherentDedispersion::set_nsamp_overlap_neg (unsigned i, unsigned n)
{
  check_index_out (i, "set_nsamp_overlap_neg");
  nsamp_overlap_neg[i] = n;
}

//! Centre frequency of input channel
double CoherentDedispersion::get_centre_frequency_input (unsigned i) const
{
  check_index_in (i, "get_centre_frequency_input");
  return centre_frequency_input[i];
}

//! Centre frequency of input channel
void CoherentDedispersion::set_centre_frequency_input (unsigned i, double f)
{
  check_index_in (i, "set_centre_frequency_input");
  centre_frequency_input[i] = f;
}

//! Centre frequency of input channel
double CoherentDedispersion::get_bandwidth_input (unsigned i) const
{
  check_index_in (i, "get_bandwidth_input");
  return bandwidth_input[i];
}

//! Centre frequency of input channel
void CoherentDedispersion::set_bandwidth_input (unsigned i, double f)
{
  check_index_in (i, "set_bandwidth_input");
  bandwidth_input[i] = f;
}

//! Centre frequency of output channel
double CoherentDedispersion::get_centre_frequency_output (unsigned i) const
{
  check_index_out (i, "get_centre_frequency_output");
  return centre_frequency_output[i];
}

//! Centre frequency of output channel
void CoherentDedispersion::set_centre_frequency_output (unsigned i, double f)
{
  check_index_out (i, "set_centre_frequency_output");
  centre_frequency_output[i] = f;
}

//! Centre frequency of output channel
double CoherentDedispersion::get_bandwidth_output (unsigned i) const
{
  check_index_out (i, "get_bandwidth_output");
  return bandwidth_output[i];
}

//! Centre frequency of output channel
void CoherentDedispersion::set_bandwidth_output (unsigned i, double f)
{
  check_index_out (i, "set_bandwidth_output");
  bandwidth_output[i] = f;
}

void CoherentDedispersion::check_index_out (unsigned i,
					    const char* method) const
{
  if (i >= nchan_output)
    throw Error (InvalidParam, "Pulsar::CoherentDedispersion::"+string(method),
		 "ichan=%u >= nchan_out=%u", i, nchan_output);
}

void CoherentDedispersion::check_index_in (unsigned i,
					   const char* method) const
{
  if (i >= nchan_input)
    throw Error (InvalidParam, "Pulsar::CoherentDedispersion::"+string(method),
		 "ichan=%u >= nchan_in=%u", i, nchan_input);
}
