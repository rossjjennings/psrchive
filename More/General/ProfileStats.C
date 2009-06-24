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
  set_onpulse_estimator (on_est);

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

//! Set the Profile from which baseline and onpulse mask will be selected
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
      onpulse[i] = 0;
}

//! The algorithm used to find the on-pulse phase bins
void Pulsar::ProfileStats::set_onpulse_estimator (OnPulseEstimator* est)
{
  onpulse_estimator = est;
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

Pulsar::OnPulseEstimator*
Pulsar::ProfileStats::get_onpulse_estimator () const
{
  return onpulse_estimator;
}
    
Pulsar::BaselineEstimator*
Pulsar::ProfileStats::get_baseline_estimator () const
{
  return baseline_estimator;
}

//! Set the on-pulse and baseline regions
void Pulsar::ProfileStats::set_regions (const PhaseWeight& on,
					const PhaseWeight& off)
{
  onpulse = on;
  baseline = off;

  regions_set = true;
  if (profile)
    build ();
}

//! Set the on-pulse and baseline regions
void Pulsar::ProfileStats::get_regions (PhaseWeight& on, 
					PhaseWeight& off) const
{
  on = onpulse;
  off = baseline;
}

//! Returns the total flux of the on-pulse phase bins
Estimate<double> Pulsar::ProfileStats::get_total (bool subtract_baseline) const
{
  double offmean = 0.0;
  
  if (subtract_baseline)
    offmean = baseline.get_mean().get_value();

  double variance = baseline.get_variance().get_value ();

  double navg = onpulse.get_weight_sum();
  double total = onpulse.get_weighted_sum();

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

unsigned Pulsar::ProfileStats::get_onpulse_nbin () const
{
  return (unsigned) onpulse.get_weight_sum();
}

unsigned Pulsar::ProfileStats::get_baseline_nbin () const
{
  return (unsigned) baseline.get_weight_sum();
}

//! Return true if the specified phase bin is in the on pulse window
bool Pulsar::ProfileStats::get_onpulse (unsigned ibin) const
{
  return onpulse[ibin];
}

void Pulsar::ProfileStats::set_onpulse (unsigned ibin, bool val)
{
  onpulse[ibin] = val;
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
Pulsar::PhaseWeight* Pulsar::ProfileStats::get_onpulse ()
{
  return &onpulse;
}

//! Return the off-pulse baseline mask
Pulsar::PhaseWeight* Pulsar::ProfileStats::get_baseline ()
{
  return &baseline;
}

//! Return the off-pulse baseline mask
Pulsar::PhaseWeight* Pulsar::ProfileStats::get_all ()
{
  return new PhaseWeight (profile.get());
}

void Pulsar::ProfileStats::build () try
{
  baseline_variance = 0.0;

  if (regions_set)
  {
    if (Profile::verbose)
      cerr << "Pulsar::ProfileStats::build regions set" << endl;
    onpulse.set_Profile (profile);
    baseline.set_Profile (profile);
    return;
  }

  onpulse_estimator->set_Profile (profile);
  onpulse_estimator->get_weight (&onpulse);
    
  baseline_estimator->set_Profile (profile);
  baseline_estimator->get_weight (&baseline);

  if (Profile::verbose)
  {
    cerr << "Pulsar::ProfileStats::build nbin=" << profile->get_nbin()
	 << " on-pulse=" << onpulse.get_weight_sum()
	 << " baseline=" << baseline.get_weight_sum() << endl;

    cerr << "onpulse ";
    for (unsigned ibin=0; ibin<profile->get_nbin(); ibin++)
      if (onpulse[ibin])
        cerr << ibin << " ";
    cerr << endl;

    cerr << "baseline ";
    for (unsigned ibin=0; ibin<profile->get_nbin(); ibin++)
      if (baseline[ibin])
        cerr << ibin << " ";
    cerr << endl;
  }
}
catch (Error& error)
{
  throw error += "Pulsar::ProfileStats::build";
}


#include "Pulsar/ProfileStatsInterface.h"

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ProfileStats::get_interface ()
{
  return new Interface (this);
}


