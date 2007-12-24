/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"

using namespace std;

//! Default constructor
Pulsar::PolnProfileStats::PolnProfileStats (const PolnProfile* _profile)
{
  set_on_pulse_estimator (new OnPulseThreshold);
  set_baseline_estimator (new GaussianBaseline);

  estimators_selected = false;
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

//! Set the PolnProfile from which baseline and on_pulse mask will be selected
/*! It is assumed that all subsequent PolnProfile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::PolnProfileStats::select_profile (const PolnProfile* set_profile)
{
  estimators_selected = false;

  if (set_profile)
    profile = set_profile;

  build ();

  if (set_profile)
    estimators_selected = true;
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

  return get_total_on_pulse ();
}

//! Returns the total linearly polarized flux of the on-pulse phase bins
Estimate<double> Pulsar::PolnProfileStats::get_total_linear () const
{
  Profile linear;
  profile->get_linear (&linear);

  setup (&linear);

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
  Profile circular;
  profile->get_circular (&circular);

  setup (&circular);

  return get_total_on_pulse ();
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
    if (on_pulse[ibin])
      total_det += invariant( get_stokes(ibin) );

  return total_det;
}

Estimate<double> Pulsar::PolnProfileStats::get_total_on_pulse () const
{
  double variance = baseline.get_variance().get_value ();
  double navg = on_pulse.get_weight_sum();

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileStats::get_total_on_pulse navg=" << navg
	 << " var=" << variance << endl;

  return Estimate<double> (on_pulse.get_weighted_sum(), 
			   variance * navg);
}

unsigned Pulsar::PolnProfileStats::get_on_pulse_nbin () const
{
  return (unsigned) on_pulse.get_weight_sum();
}

unsigned Pulsar::PolnProfileStats::get_baseline_nbin () const
{
  return (unsigned) baseline.get_weight_sum();
}

Estimate<double>
Pulsar::PolnProfileStats::get_baseline_variance (unsigned ipol) const
{
  if (baseline_variance[ipol].get_value() == 0)
  {
    baseline.set_Profile( profile->get_Profile(ipol) );
    baseline_variance[ipol] = baseline.get_variance();
  }
  return baseline_variance[ipol];
}


void Pulsar::PolnProfileStats::build () try
{
  if (profile->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::PolnProfileStats::build",
		 "input PolnProfile is not in the Stokes state");

  baseline_variance = Stokes< Estimate<double> > ();

  if (estimators_selected)
    return;

  on_pulse_estimator->set_Profile (profile->get_Profile(0));
  on_pulse_estimator->get_weight (&on_pulse);
    
  baseline_estimator->set_Profile (profile->get_Profile(0));
  baseline_estimator->get_weight (&baseline);

  if (Profile::verbose)
    cerr << "Pulsar::PolnProfileStats::build nbin=" << profile->get_nbin()
	 << " on-pulse=" << on_pulse.get_weight_sum()
	 << " baseline=" << baseline.get_weight_sum() << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileStats::build";
}

void Pulsar::PolnProfileStats::setup (const Profile* prof) const
{
  on_pulse.set_Profile (prof);
  baseline.set_Profile (prof);
}
