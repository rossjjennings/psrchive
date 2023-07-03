/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnSpectrumStats.h"

using namespace std;

//! Get the Stokes parameters for the specified harmonic
Stokes< std::complex< Estimate<double> > > 
Pulsar::PolnSpectrumStats::get_stokes (unsigned ibin) const
{
  std::complex< Estimate<double> > zero ( 0.0, 0.0 );
  Stokes< std::complex< Estimate<double> > > result ( zero, zero, zero, zero );

  Stokes< Estimate<double> > re = real->get_stokes (ibin);
  Stokes< Estimate<double> > im = imag->get_stokes (ibin);

  for (unsigned ipol=0; ipol < 4; ipol++)
    result[ipol] = std::complex< Estimate<double> > (re[ipol], im[ipol]);

  return result;
}

//! Returns the total determinant of the on-pulse phase bins
Estimate<double> Pulsar::PolnSpectrumStats::get_total_determinant () const
{
  return real->get_total_determinant() + imag->get_total_determinant();
}

//! Returns the variance of the baseline for the specified polarization
std::complex< Estimate<double> >
Pulsar::PolnSpectrumStats::get_baseline_variance (unsigned ipol) const
{
  // note that real and imag store the baseline mask passed to their set_regions method
  return std::complex< Estimate<double> >
    ( real->get_baseline_variance(ipol), imag->get_baseline_variance(ipol) );
}

void Pulsar::PolnSpectrumStats::preprocess (PolnProfile* fourier)
{
  // convert to Stokes parameters
  fourier->convert_state (Signal::Stokes);
}
