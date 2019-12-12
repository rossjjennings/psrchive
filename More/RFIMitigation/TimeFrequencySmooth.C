/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeFrequencySmooth.h"

Pulsar::TimeFrequencySmooth::TimeFrequencySmooth ()
{
}

void Pulsar::TimeFrequencySmooth::check_dimensions(std::vector<float> &smoothed,
    std::vector<float> &raw, std::vector<float> &weight,
    std::vector<float> &freqs, std::vector<float> &times)
{
  // Determine dimensions
  nsub = times.size();
  nchan = freqs.size() / nsub;
  npol = raw.size() / nchan / nsub;

  // Check for dimension problems
  if (freqs.size() % nsub) 
    throw Error (InvalidParam, "Pulsar::TimeFrequencySmooth::check_dimensions",
        "freq array bad size (size=%d nsub=%d)", freqs.size(), nsub);

  if (weight.size() != nsub*nchan)
    throw Error (InvalidParam, "Pulsar::TimeFrequencySmooth::check_dimensions",
        "weight array bad size");

  if (raw.size() % (nsub*nchan))
    throw Error (InvalidParam, "Pulsar::TimeFrequencySmooth::check_dimensions",
        "raw array bad size");

  smoothed.resize(nsub*nchan*npol);
}

