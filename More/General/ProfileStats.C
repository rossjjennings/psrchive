/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStats.h"
#include "Pulsar/Profile.h"

#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/GaussianBaseline.h"

using namespace std;

//! Default constructor
Pulsar::ProfileStats::ProfileStats (const Profile* _profile)
{
  PeakConsecutive* on_est = new PeakConsecutive;
  set_on_pulse_estimator (on_est);

  GaussianBaseline* off_est = new GaussianBaseline;
  set_baseline_estimator (off_est);
  on_est->set_baseline_estimator (off_est);

  regions_set = false;
  set_profile (_profile);
}

//! Destructor
Pulsar::ProfileStats::~ProfileStats()
{
}

//! Set the Profile from which statistics will be derived
void Pulsar::ProfileStats::set_profile (const Profile* _profile)
{
  profile = _profile;
  if (profile)
    build ();
}

//! Set the Profile from which baseline and on_pulse mask will be selected
/*! It is assumed that all subsequent Profile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::ProfileStats::select_profile (const Profile* set_profile)
{
  regions_set = false;

  if (set_profile)
    profile = set_profile;

  build ();

  if (set_profile)
    regions_set = true;
}

void Pulsar::ProfileStats::deselect_onpulse (const Profile* prof, float thresh)
{
  for (unsigned i=0; i<prof->get_nbin(); i++)
    if (prof->get_amps()[i] < thresh)
      on_pulse[i] = 0;
}

//! The algorithm used to find the on-pulse phase bins
void Pulsar::ProfileStats::set_on_pulse_estimator (OnPulseEstimator* est)
{
  on_pulse_estimator = est;
  if (profile)
    build ();
}

//! The algorithm used to find the off-pulse phase bins
void Pulsar::ProfileStats::set_baseline_estimator (BaselineEstimator* est)
{
  baseline_estimator = est;
  if (profile)
    build ();
}

//! Set the on-pulse and baseline regions
void Pulsar::ProfileStats::set_regions (const PhaseWeight& on,
					const PhaseWeight& off)
{
  on_pulse = on;
  baseline = off;

  regions_set = true;
  if (profile)
    build ();
}

//! Set the on-pulse and baseline regions
void Pulsar::ProfileStats::get_regions (PhaseWeight& on, 
					PhaseWeight& off) const
{
  on = on_pulse;
  off = baseline;
}

//! Returns the total flux of the on-pulse phase bins
Estimate<double> Pulsar::ProfileStats::get_total (bool subtract_baseline) const
{
  double offmean = 0.0;
  
  if (subtract_baseline)
    offmean = baseline.get_mean().get_value();

  double variance = baseline.get_variance().get_value ();

  double navg = on_pulse.get_weight_sum();
  double total = on_pulse.get_weighted_sum();

#if 0
  navg = profile->get_nbin();
  total = profile->sum();
#endif

  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::get_total"
         << "\nt on nbin=" << navg << " tot=" << total 
	 << "\nt off mean=" << offmean << " var=" << variance << endl;

  return Estimate<double> (total - offmean * navg, variance * navg);
}

unsigned Pulsar::ProfileStats::get_on_pulse_nbin () const
{
  return (unsigned) on_pulse.get_weight_sum();
}

unsigned Pulsar::ProfileStats::get_baseline_nbin () const
{
  return (unsigned) baseline.get_weight_sum();
}

//! Return true if the specified phase bin is in the on pulse window
bool Pulsar::ProfileStats::get_on_pulse (unsigned ibin) const
{
  return on_pulse[ibin];
}

void Pulsar::ProfileStats::set_on_pulse (unsigned ibin, bool val)
{
  on_pulse[ibin] = val;
}

//! Return true if the specified phase bin is in the baseline window
bool Pulsar::ProfileStats::get_baseline (unsigned ibin) const
{
  return baseline[ibin];
}

Estimate<double> Pulsar::ProfileStats::get_baseline_variance () const
{
  if (baseline_variance.get_value() == 0)
    baseline_variance = baseline.get_variance();
  return baseline_variance;
}

//! Return the on-pulse phase bin mask
const Pulsar::PhaseWeight* Pulsar::ProfileStats::get_on_pulse ()
{
  return &on_pulse;
}

//! Return the off-pulse baseline mask
const Pulsar::PhaseWeight* Pulsar::ProfileStats::get_baseline ()
{
  return &baseline;
}

void Pulsar::ProfileStats::build () try
{
  baseline_variance = 0.0;

  if (regions_set)
  {
    if (Profile::verbose)
      cerr << "Pulsar::ProfileStats::build regions set" << endl;
    on_pulse.set_Profile (profile);
    baseline.set_Profile (profile);
    return;
  }

  on_pulse_estimator->set_Profile (profile);
  on_pulse_estimator->get_weight (&on_pulse);
    
  baseline_estimator->set_Profile (profile);
  baseline_estimator->get_weight (&baseline);

  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::build nbin=" << profile->get_nbin()
	 << " on-pulse=" << on_pulse.get_weight_sum()
	 << " baseline=" << baseline.get_weight_sum() << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::ProfileStats::build";
}

