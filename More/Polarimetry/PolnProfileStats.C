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

// #define _DEBUG

//! Default constructor
Pulsar::PolnProfileStats::PolnProfileStats (const PolnProfile* _profile)
{
  avoid_zero_determinant = false;
  regions_set = false;
  stats = new ProfileStats;
  set_profile (_profile);
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
void Pulsar::PolnProfileStats::set_profile (const PolnProfile* _profile)
{
  profile = _profile;
  build ();
}

//! Set the PolnProfile from which baseline and on_pulse mask will be selected
/*! It is assumed that all subsequent PolnProfile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::PolnProfileStats::select_profile (const PolnProfile* _profile)
{
  profile = _profile;
  regions_set = false;
  build ();
  if (profile)
    regions_set = true;
}

void Pulsar::PolnProfileStats::select_profile (const Profile* total)
{
  profile = 0;
  stats->select_profile( total );
  regions_set = true;
}

//! Set the on-pulse and baseline regions
void Pulsar::PolnProfileStats::set_regions (const PhaseWeight& on,
					    const PhaseWeight& off)
{
  stats->set_regions (on, off);
  regions_set = true;
  build ();
}

//! Set the on-pulse and baseline regions
void Pulsar::PolnProfileStats::get_regions (PhaseWeight& on, 
					    PhaseWeight& off) const
{
  stats->get_regions (on, off);
}

//! Returns the total flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_intensity () const
{
  stats->set_profile( profile->get_Profile(0) );
  return stats->get_total ();
}


//! Returns the total polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_polarized () const
{
  Profile polarized;
  profile->get_polarized (&polarized);

  stats->set_profile (&polarized);

  return stats->get_total (false);
}

//! Returns the total linearly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_linear () const
{
  Profile linear;
  profile->get_linear (&linear);

  stats->set_profile (&linear);

  return stats->get_total (false);
}

//! Returns the total circularly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_circular () const
{
  stats->set_profile (profile->get_Profile(3));
  return stats->get_total ();
}

//! Returns the total absolute value of circularly polarized flux
Estimate<double> Pulsar::PolnProfileStats::get_total_abs_circular () const
{
  Profile circular;
  profile->get_circular (&circular);

  stats->set_profile (&circular);

  return stats->get_total (false);
}

//! Get the Stokes parameters of the specified phase bin
Stokes< Estimate<double> >
Pulsar::PolnProfileStats::get_stokes (unsigned ibin) const
{
  Stokes< Estimate<double> > result = profile->get_Stokes (ibin);

  for (unsigned ipol=0; ipol < 4; ipol++)
    result[ipol].set_variance( get_baseline_variance(ipol).get_value() );

  return result;
}

//! Returns the total determinant of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_determinant () const
{
  Estimate<double> total_det;

  for (unsigned ibin=0; ibin < profile->get_nbin(); ibin++)
    if (stats->get_on_pulse(ibin))
      total_det += invariant( get_stokes(ibin) );

#if 0
  cerr << "1: " << total_det << endl;

  Profile invint;
  profile->invint (&invint, false);

  stats->set_profile (&invint);

  total_det = stats->get_total (false);

  cerr << "2: " << total_det << endl;
#endif

  return total_det;
}

Estimate<double>
Pulsar::PolnProfileStats::get_baseline_variance (unsigned ipol) const
{
  if (baseline_variance[ipol].get_value() == 0)
  {
    stats->set_profile( profile->get_Profile(ipol) );
    baseline_variance[ipol] = stats->get_baseline_variance();
#ifdef _DEBUG
    cerr << "Pulsar::PolnProfileStats::get_baseline_variance ipol="
         << ipol << " var=" << baseline_variance[ipol] << endl;
#endif
  }
  return baseline_variance[ipol];
}


void Pulsar::PolnProfileStats::build () try
{
  baseline_variance = Stokes< Estimate<double> > ();

  if (!profile)
    return;

  if (profile->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::PolnProfileStats::build",
		 "input PolnProfile is not in the Stokes state");

  if (!regions_set)
  {
    stats->select_profile( profile->get_Profile(0) );

    if (avoid_zero_determinant)
    {
      Profile invint;
      profile->invint (&invint);

      // roughly speaking ...
      double invint_variance = get_baseline_variance(0).val * 4;
      double threshold = 3.0 * sqrt (invint_variance);

#ifdef _DEBUG
      cerr << "before avoid " << stats->get_on_pulse_nbin() << endl;
#endif
      stats->deselect_onpulse (&invint, threshold);
#ifdef _DEBUG
      cerr << "after avoid " << stats->get_on_pulse_nbin() << endl;
#endif
    }
  }
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::build";
}

