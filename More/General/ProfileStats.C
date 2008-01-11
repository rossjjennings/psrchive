/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProfileStats.h"
#include "Pulsar/Profile.h"

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"

using namespace std;

//! Default constructor
Pulsar::ProfileStats::ProfileStats (const Profile* _profile)
{
  set_on_pulse_estimator (new OnPulseThreshold);
  set_baseline_estimator (new GaussianBaseline);

  estimators_selected = false;
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
  estimators_selected = false;

  if (set_profile)
    profile = set_profile;

  build ();

  if (set_profile)
    estimators_selected = true;
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

//! Returns the total flux of the on-pulse phase bins
Estimate<double> Pulsar::ProfileStats::get_total () const
{
  double variance = baseline.get_variance().get_value ();
  double navg = on_pulse.get_weight_sum();

  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::get_total_on_pulse navg=" << navg
	 << " var=" << variance << endl;

  return Estimate<double> (on_pulse.get_weighted_sum(), 
			   variance * navg);
}

unsigned Pulsar::ProfileStats::get_on_pulse_nbin () const
{
  return (unsigned) on_pulse.get_weight_sum();
}

unsigned Pulsar::ProfileStats::get_baseline_nbin () const
{
  return (unsigned) baseline.get_weight_sum();
}

Estimate<double> Pulsar::ProfileStats::get_baseline_variance () const
{
  if (baseline_variance.get_value() == 0)
    baseline_variance = baseline.get_variance();
  return baseline_variance;
}


void Pulsar::ProfileStats::build () try
{
  baseline_variance = 0.0;

  if (estimators_selected)
  {
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

