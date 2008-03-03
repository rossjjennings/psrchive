/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSpectra.h"
#include "Pulsar/PolnProfileStats.h"

//! Default constructor
/*! If specified, baseline and on-pulse regions are defined by select */
Calibration::StandardSpectra::StandardSpectra (const Pulsar::PolnProfile* pp)
{
  stats = new Pulsar::PolnSpectrumStats;

  if (pp)
    select_profile (pp);
}

//! Select the baseline and on-pulse regions from profile
void
Calibration::StandardSpectra::select_profile (const Pulsar::PolnProfile* pp)
{
  stats->select_profile (pp);
}



//! Set the profile from which estimates will be derived
void Calibration::StandardSpectra::set_profile (const Pulsar::PolnProfile* p)
{
  stats->set_profile (p);
  total_determinant = stats->get_total_determinant ();
}

//! Normalize estimates by the average determinant
void Calibration::StandardSpectra::set_normalize (bool norm)
{
  if (norm)
    normalize = new MEAL::NormalizeStokes;
  else
    normalize = 0;
}

//! Get the Stokes parameters of the specified phase bin
Stokes< std::complex< Estimate<double> > >
Calibration::StandardSpectra::get_stokes (unsigned ibin)
{
  Stokes< Estimate<double> > re = stats->get_real()->get_stokes (ibin);
  Stokes< Estimate<double> > im = stats->get_imag()->get_stokes (ibin);

  if (normalize)
  {
    normalize->normalize (re, total_determinant);
    normalize->normalize (im, total_determinant);

    unsigned nbin = stats->get_real()->get_stats()->get_on_pulse_nbin();

    re *= sqrt( nbin );
    im *= sqrt( nbin );
  }

  std::complex< Estimate<double> > zero;
  Stokes< std::complex< Estimate<double> > > result( zero, zero, zero, zero );

  for (unsigned i=0; i<4; i++)
    result[i] = std::complex< Estimate<double> > (re[i], im[i]);

  return result;
}
