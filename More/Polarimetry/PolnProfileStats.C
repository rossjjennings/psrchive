/***************************************************************************
 *
 *   Copyright (C) 2005-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"

using namespace std;

// #define _DEBUG 1

//! Default constructor
Pulsar::PolnProfileStats::PolnProfileStats (const PolnProfile* _profile) try
{
  avoid_zero_determinant = false;
  regions_set = false;
  stats = new ProfileStats;

  DEBUG("Pulsar::PolnProfileStats::ctor this=" << this << " calling select_profile");

  select_profile (_profile);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::ctor";
}

//! Destructor
Pulsar::PolnProfileStats::~PolnProfileStats()
{
}

void Pulsar::PolnProfileStats::set_avoid_zero_determinant (bool flag)
{
  avoid_zero_determinant = flag;
}

//! Set the PolnProfile from which statistics will be derived
void Pulsar::PolnProfileStats::set_profile (const PolnProfile* _profile) try
{
  profile.set(_profile);

  DEBUG("Pulsar::PolnProfileStats::set_profile this=" << this << " calling build");
  build ();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::set_profile";
}

//! Set the PolnProfile from which baseline and onpulse mask will be selected
/*! It is assumed that all subsequent PolnProfile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::PolnProfileStats::select_profile (const PolnProfile* _profile) try
{
  DEBUG("Pulsar::PolnProfileStats::select_profile this=" << this << " PolnProfile=" << _profile);

  profile.set(_profile);
  regions_set = false;
  build ();
  if (_profile)
    regions_set = true;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::select_profile (PolnProfile*)";
}

void Pulsar::PolnProfileStats::select_profile (const Profile* total) try
{
  DEBUG("Pulsar::PolnProfileStats::select_profile this=" << this << " Profile=" << total);

  profile = 0;
  stats->select_profile( total );
  regions_set = true;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::select_profile (Profile*)";
}

//! Set the on-pulse and baseline regions
void Pulsar::PolnProfileStats::set_regions (const PhaseWeight& on, const PhaseWeight& off) try
{
  stats->set_regions (on, off);
  regions_set = true;
  build ();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::set_regions";
}

//! Set the on-pulse and baseline regions
void Pulsar::PolnProfileStats::get_regions (PhaseWeight& on, PhaseWeight& off) const try
{
  stats->get_regions (on, off);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_regions";
}

//! Returns the total flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_intensity () const try
{
  stats->set_profile( profile->get_Profile(0) );
  return stats->get_total ();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_intensity";
}

//! Returns the total polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_polarized () const try
{
  Profile polarized;
  profile->get_polarized (&polarized);

  stats->set_profile (&polarized);

  return stats->get_total (false);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_polarized";
}

//! Returns the total polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_polarized_squared () const try
{
  Profile polarized;
  profile->get_polarized_squared (&polarized);

  stats->set_profile (&polarized);

  return stats->get_total (false);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_polarized_squared";
}

//! Returns the total linearly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_linear () const try
{
  Profile linear;
  profile->get_linear (&linear);

  DEBUG("Pulsar::PolnProfileStats::get_total_linear this=" << this << 
        " calling ProfileStats::set_profile regions_set=" << regions_set);

  stats->set_profile (&linear);

  DEBUG("Pulsar::PolnProfileStats::get_total_linear this=" << this <<
        " calling ProfileStats::get_total");

  return stats->get_total (false);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_linear";
}

//! Returns the total linearly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_linear_squared () const try
{
  Profile linear;
  profile->get_linear_squared (&linear);

  DEBUG("Pulsar::PolnProfileStats::get_total_linear_squared this=" << this << 
        " calling ProfileStats::set_profile regions_set=" << regions_set);

  stats->set_profile (&linear);

  DEBUG("Pulsar::PolnProfileStats::get_total_linear_squared this=" << this <<
        " calling ProfileStats::get_total");

  return stats->get_total (false);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_linear_squared ";
}

//! Returns the total circularly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_circular () const try
{
  stats->set_profile (profile->get_Profile(3));
  return stats->get_total ();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_circular";
}

//! Returns the total absolute value of circularly polarized flux
Estimate<double> Pulsar::PolnProfileStats::get_total_abs_circular () const try
{
  Profile circular;
  profile->get_circular (&circular);

  stats->set_profile (&circular);

  return stats->get_total (false);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::total_abs_circular";
}

//! Get the Stokes parameters of the specified phase bin
Stokes<Estimate<double>>
Pulsar::PolnProfileStats::get_stokes (unsigned ibin) const try
{
  Stokes<Estimate<double>> result = profile->get_Stokes (ibin);

  for (unsigned ipol=0; ipol < 4; ipol++)
    result[ipol].set_variance( get_baseline_variance(ipol).get_value() );

  return result;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_stokes";
}

//! Returns the total of the squared invariant of the on-pulse phase bins
/*! The average Stokes parameters returned by get_baseline are subtracted from 
    the Stokes parameters in each on-pulse phase bin before computing the invariant.
 */
Estimate<double> Pulsar::PolnProfileStats::get_total_squared_invariant () const try
{
  Estimate<double> total_inv_squared;
  Stokes<Estimate<double>> baseline = get_baseline();

  for (unsigned ibin=0; ibin < profile->get_nbin(); ibin++)
  {
    if (stats->get_onpulse(ibin))
    {
      Stokes<Estimate<double>> S = get_stokes(ibin) - baseline;
      total_inv_squared += invariant(S);
    }
  }

  return total_inv_squared;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_squared_invariant";
}

Estimate<double> Pulsar::PolnProfileStats::get_invariant() const
{
  Estimate<double> total_inv_squared = get_total_squared_invariant ();
  return sqrt(total_inv_squared);
}

//! Returns the total determinant of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_determinant () const try
{
  // det(rho) = |S|^2 / 4
  return 0.25 * get_total_squared_invariant();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_total_determinant";
}

//! Get the mean Stokes parameters of the baseline phase bins
Stokes<Estimate<double>> Pulsar::PolnProfileStats::get_baseline() const
{
  if (baseline_mean[0].get_value() == 0)
  {
    DEBUG("Pulsar::PolnProfileStats::get_baseline recomputing baseline");

    for (unsigned ipol=0; ipol < 4; ipol++)
    {
      stats->set_profile( profile->get_Profile(ipol) );
      baseline_mean[ipol] = stats->get_baseline_mean();
      DEBUG("Pulsar::PolnProfileStats::get_baseline ipol="
            << ipol << " baseline=" << baseline_mean[ipol]);
    }
  }
  return baseline_mean;
}

Estimate<double>
Pulsar::PolnProfileStats::get_baseline_variance (unsigned ipol) const try
{
  if (baseline_variance[ipol].get_value() == 0)
  {
    stats->set_profile( profile->get_Profile(ipol) );
    baseline_variance[ipol] = stats->get_baseline_variance();
    DEBUG("Pulsar::PolnProfileStats::get_baseline_variance ipol="
          << ipol << " var=" << baseline_variance[ipol]);
  }
  return baseline_variance[ipol];
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::get_baseline_variance";
}

void Pulsar::PolnProfileStats::build () try
{
  baseline_variance = Stokes<Estimate<double>> ();
  baseline_mean = Stokes<Estimate<double>> ();

  if (!profile)
    return;

  if (profile->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::PolnProfileStats::build",
		 "input PolnProfile is not in the Stokes state");

  if (!regions_set)
  {
    DEBUG("Pulsar::PolnProfileStats::build this=" << this << 
          " call ProfileStats::select_profile"
          " ProfileStats*=" << stats.ptr());

    stats->select_profile( profile->get_Profile(0) );

    if (avoid_zero_determinant)
    {
      Profile invint;
      profile->invint (&invint);

      // roughly speaking ...
      double invint_variance = get_baseline_variance(0).val * 4;
      double threshold = 3.0 * sqrt (invint_variance);

      DEBUG("before avoid " << stats->get_onpulse_nbin());
      stats->deselect_onpulse (&invint, threshold);
      DEBUG("after avoid " << stats->get_onpulse_nbin());
    }
  }
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::build";
}

//! Returns the vairance of the linearly polarized flux
double Pulsar::PolnProfileStats::get_linear_variance () const
{
  return profile->get_linear_variance();
}

#include "Pulsar/PolnProfileStatsInterface.h"

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PolnProfileStats::get_interface ()
{
  return new Interface (this);
}


