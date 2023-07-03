/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluctuationSpectrumStats.h"

// #define _DEBUG 1
#include "debug.h"

using namespace std;

//! Returns the variance of the baseline for the specified polarization
std::complex< Estimate<double> >
Pulsar::FluctuationSpectrumStats::get_baseline_variance () const
{
  // note that real and imag store the baseline mask passed to their set_regions method
  return std::complex< Estimate<double> >
    ( real->get_baseline_variance(), imag->get_baseline_variance() );
}

//! Returns the total variance of the noise
double Pulsar::FluctuationSpectrumStats::get_noise_variance () const
{
  return real->get_baseline_variance().get_value() + imag->get_baseline_variance().get_value();
}

void copy_pad (Pulsar::PhaseWeight& into, Pulsar::PhaseWeight& from, float remainder)
{
  const unsigned from_nbin = from.get_nbin();
  const unsigned into_nbin = into.get_nbin();

  unsigned ibin = 0;
  for (; ibin < from_nbin; ibin++)
    into[ibin] = from[ibin];

  for (; ibin < into_nbin; ibin++)
    into[ibin] = remainder;
}