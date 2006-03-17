/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/BaselineEstimator.h"

//! Default constructor
Pulsar::PolnProfileStats::PolnProfileStats (const PolnProfile* _profile)
{
  OnPulseThreshold* threshold = new OnPulseThreshold;

  set_on_pulse_estimator (threshold);
  set_baseline_estimator (threshold->get_baseline_estimator());

  set_profile (_profile);
}

//! Destructor
Pulsar::PolnProfileStats::~PolnProfileStats()
{
}

//! Set the PolnProfile from which statistics will be derived
void Pulsar::PolnProfileStats::set_profile (const PolnProfile* _profile)
{
  profile = _profile;
  if (profile)
    build ();
}

//! The algorithm used to find the on-pulse phase bins
void Pulsar::PolnProfileStats::set_on_pulse_estimator (OnPulseEstimator* est)
{
  on_pulse_estimator = est;
  if (profile)
    build ();
}

//! The algorithm used to find the off-pulse phase bins
void Pulsar::PolnProfileStats::set_baseline_estimator (BaselineEstimator* est)
{
  baseline_estimator = est;
  if (profile)
    build ();
}

//! Returns the total flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_intensity () const
{
  setup (profile->get_Profile(0));
  return get_total_on_pulse ();
}


//! Returns the total polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_polarized () const
{
  Profile polarized;
  profile->get_polarized (&polarized);

  setup (&polarized);
  polarized -= baseline.get_mean().get_value();

  return get_total_on_pulse ();
}

//! Returns the total linearly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_linear () const
{
  Profile linear;
  profile->get_linear (&linear);

  setup (&linear);
  linear -= baseline.get_mean().get_value();

  return get_total_on_pulse ();
}

//! Returns the total circularly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_circular () const
{
  setup (profile->get_Profile(3));
  return get_total_on_pulse ();
}

//! Returns the total absolute value of circularly polarized flux
Estimate<double> Pulsar::PolnProfileStats::get_total_abs_circular () const
{
  Profile abs_circular (*(profile->get_Profile(3)));
  abs_circular.absolute ();

  setup (&abs_circular);
  abs_circular -= baseline.get_mean().get_value();

  return get_total_on_pulse ();
}

Estimate<double> Pulsar::PolnProfileStats::get_total_on_pulse () const
{
  double variance = baseline.get_variance().get_value ();
  double navg = on_pulse.get_weight_sum();

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileStats::get_total_on_pulse navg=" << navg
	 << " var=" << variance << endl;

  return Estimate<double> (on_pulse.get_weighted_sum(), 
			   variance * sqrt(navg));
}


void Pulsar::PolnProfileStats::build ()
try {

  if (profile->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::PolnProfileStats::build",
		 "input PolnProfile is not in the Stokes state");

  on_pulse_estimator->set_Profile (profile->get_Profile(0));
  on_pulse_estimator->get_weight (on_pulse);

  baseline_estimator->set_Profile (profile->get_Profile(0));
  baseline_estimator->get_weight (baseline);

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileStats::build nbin=" << profile->get_nbin()
	 << " on-pulse=" << on_pulse.get_weight_sum()
	 << " baseline=" << baseline.get_weight_sum() << endl;

}
catch (Error& error) {
  throw error += "Pulsar::PolnProfileStats::build";
}

void Pulsar::PolnProfileStats::setup (const Profile* prof) const
{
  PolnProfileStats* thiz = const_cast<PolnProfileStats*>(this);

  thiz->on_pulse.set_Profile (prof);
  thiz->baseline.set_Profile (prof);
}
