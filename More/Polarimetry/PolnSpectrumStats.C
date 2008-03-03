/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnSpectrumStats.h"

#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/Fourier.h"
#include "Pulsar/LastHarmonic.h"
#include "Pulsar/ExponentialBaseline.h"

using namespace std;

//! Default constructor
Pulsar::PolnSpectrumStats::PolnSpectrumStats (const PolnProfile* _profile)
{
  real = new Pulsar::PolnProfileStats;
  imag = new Pulsar::PolnProfileStats;

  regions_set = false;
  last_harmonic = 0;

  if (profile)
    select_profile (_profile);
}

//! Destructor
Pulsar::PolnSpectrumStats::~PolnSpectrumStats()
{
}

//! Set the PolnProfile from which statistics will be derived
void Pulsar::PolnSpectrumStats::set_profile (const PolnProfile* _profile)
{
  profile = _profile;
  build ();
}

//! Set the PolnProfile from which baseline and on_pulse mask will be selected
/*! It is assumed that all subsequent PolnProfile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::PolnSpectrumStats::select_profile (const PolnProfile* _profile)
{
  profile = _profile;
  regions_set = false;
  build ();
  if (_profile)
    regions_set = true;
}

//! Get the fourier transform of the last set profile
const Pulsar::PolnProfile* Pulsar::PolnSpectrumStats::get_fourier () const
{
  return fourier;
}

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

//! Get the real componentStokes parameters for the specified harmonic
const Pulsar::PolnProfileStats*
Pulsar::PolnSpectrumStats::get_real () const
{
  return real;
}

//! Get the imaginary componentStokes parameters for the specified harmonic
const Pulsar::PolnProfileStats*
Pulsar::PolnSpectrumStats::get_imag () const
{
  return imag;
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
  return std::complex< Estimate<double> >
    ( real->get_baseline_variance(ipol), imag->get_baseline_variance(ipol) );
}

Pulsar::PolnProfile* fourier_to_psd (const Pulsar::PolnProfile* fourier)
{
  Reference::To<Pulsar::PolnProfile> psd = fourier->clone();
  detect (psd);
  return psd.release();
}

void Pulsar::PolnSpectrumStats::build () try
{
  if (!profile)
    return;

  fourier = fourier_transform (profile, plan);

  // convert to Stokes parameters and drop the Nyquist bin
  fourier->convert_state (Signal::Stokes);
  fourier->resize( profile->get_nbin() );

  // form the power spectral density
  Reference::To<PolnProfile> psd = fourier_to_psd (fourier);

  // separate fourier into real and imaginary components
  Reference::To<PolnProfile> re = psd->clone();
  Reference::To<PolnProfile> im = psd->clone();

  unsigned npol = 4;
  unsigned nbin = psd -> get_nbin();

  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    float* C_ptr = fourier->get_Profile(ipol)->get_amps();
    float* re_ptr = re->get_Profile(ipol)->get_amps();
    float* im_ptr = re->get_Profile(ipol)->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      re_ptr[ibin] = C_ptr[ibin*2];
      im_ptr[ibin] = C_ptr[ibin*2+1];
    }
  }

  if (!regions_set)
  {
    LastHarmonic last;
    last.set_Profile( psd->get_Profile(0) );

    last_harmonic = last.get_last_harmonic();

    PhaseWeight on;
    last.get_weight (&on);

    PhaseWeight off;
    last.get_baseline_estimator()->get_weight (&off);

    real->get_stats()->set_regions (on, off);
    imag->get_stats()->set_regions (on, off);

    regions_set = true;
  }

  real->set_profile (re);
  imag->set_profile (im);

}
catch (Error& error)
{
  throw error += "Pulsar::PolnSpectrumStats::build";
}
