/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStats.h"
#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/Profile.h"

#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/SNRatioEstimator.h"
#include "Pulsar/PhaseWidth.h"

using namespace std;

//! Default constructor
Pulsar::ProfileStats::ProfileStats (const Profile* _profile)
{
  PeakConsecutive* on_est = new PeakConsecutive;
  set_onpulse_estimator (on_est);

  GaussianBaseline* off_est = new GaussianBaseline;
  set_baseline_estimator (off_est);
  on_est->set_baseline_estimator (off_est);

  width_estimator = new PhaseWidth;

  built = false;
  regions_set = false;
  set_profile (_profile);
}

Pulsar::ProfileStats::ProfileStats (const ProfileStats& copy) : HasBaselineEstimator (copy)
{
  if (copy.onpulse_estimator)
    onpulse_estimator = copy.onpulse_estimator->clone();

  if (copy.snratio_estimator)
    snratio_estimator = copy.snratio_estimator->clone();

  if (copy.width_estimator)
    width_estimator = copy.width_estimator->clone();

  if (copy.include_estimator)
    include_estimator = copy.include_estimator->clone();

  if (copy.exclude_estimator)
    exclude_estimator = copy.exclude_estimator->clone();

  built = false;
  regions_set = false;
}

//! Destructor
Pulsar::ProfileStats::~ProfileStats()
{
}

Pulsar::ProfileStats* Pulsar::ProfileStats::clone () const
{
  return new ProfileStats (*this);
}


//! The algorithm used to find the bins to include
void Pulsar::ProfileStats::set_include_estimator (ProfileWeightFunction* est)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_include this=" << this
	 << " est=" << est << endl;

  include_estimator = est;
  built = false;
  regions_set = false;
}

Pulsar::ProfileWeightFunction*
Pulsar::ProfileStats::get_include_estimator () const
{
  return include_estimator.ptr();
}

//! The algorithm used to find the bins to exclude
void Pulsar::ProfileStats::set_exclude_estimator (ProfileWeightFunction* est)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_exclude this=" << this
	 << " est=" << est << endl;

  exclude_estimator = est;
  built = false;
  regions_set = false;
}

Pulsar::ProfileWeightFunction*
Pulsar::ProfileStats::get_exclude_estimator () const
{
  return exclude_estimator.ptr();
}

//! Set the Profile from which statistics will be derived
void Pulsar::ProfileStats::set_profile (const Profile* _profile)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_profile this=" << this
	 << " profile=" << _profile << " regions_set=" << regions_set << endl;

  profile = _profile;
  built = false;
}

//! Set the Profile from which baseline and onpulse mask will be selected
/*! It is assumed that all subsequent Profile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::ProfileStats::select_profile (const Profile* set_profile) try
{ 
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::select_profile this=" << this 
	 << " Profile=" << set_profile << endl;

  regions_set = false;

  if (set_profile)
    profile = set_profile;

  build ();

  if (set_profile)
    regions_set = true;
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::select_profile";
   }

void Pulsar::ProfileStats::deselect_onpulse (const Profile* prof, float thresh)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::deselect_onpulse prof=" << (void*) prof << endl;

  if (!prof)
    return;

  if (prof->get_nbin() != onpulse.get_nbin())
    throw Error (InvalidState, "Pulsar::ProfileStats::deselect_onpulse",
                 "profile nbin=%u != onpulse nbin=%u", 
                 prof->get_nbin(), onpulse.get_nbin());

  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::deselect_onpulse nbin=" << prof->get_nbin() 
         << endl;

  for (unsigned i=0; i<prof->get_nbin(); i++)
    if (prof->get_amps()[i] < thresh)
      onpulse[i] = 0;
}

//! The algorithm used to find the on-pulse phase bins
void Pulsar::ProfileStats::set_onpulse_estimator (ProfileWeightFunction* est)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_onpulse_estimator this=" << this
	 << " est=" << est << endl;

  onpulse_estimator = est;
  built = false;
  regions_set = false;
}

Pulsar::ProfileWeightFunction*
Pulsar::ProfileStats::get_onpulse_estimator () const
{
  return onpulse_estimator;
}

//! The algorithm used to find the off-pulse phase bins
void Pulsar::ProfileStats::set_baseline_estimator (ProfileWeightFunction* est)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_baseline_estimator this=" << this
	 << " est=" << est << endl;

  HasBaselineEstimator::set_baseline_estimator (est);
  built = false;
  regions_set = false;
}

void Pulsar::ProfileStats::set_width_estimator (WidthEstimator* est)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_width_estimator this=" << this
         << " est=" << est << endl;

  width_estimator = est;
  built = false;
}

Pulsar::WidthEstimator*
Pulsar::ProfileStats::get_width_estimator () const
{
  return width_estimator;
}

void Pulsar::ProfileStats::set_snratio_estimator (SNRatioEstimator* est)
{
  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_snratio_estimator this=" << this
         << " est=" << est << endl;

  snratio_estimator = est;
  built = false;
}

Pulsar::SNRatioEstimator*
Pulsar::ProfileStats::get_snratio_estimator () const
{
  return snratio_estimator;
}

 
//! Set the on-pulse and baseline regions
void Pulsar::ProfileStats::set_regions (const PhaseWeight& on,
					const PhaseWeight& off)
{
  onpulse = on;
  baseline = off;

  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::set_regions true" << endl;

  regions_set = true;
  built = false;
}

//! Set the on-pulse and baseline regions
void Pulsar::ProfileStats::get_regions (PhaseWeight& on, 
					PhaseWeight& off) const
{
  on = onpulse;
  off = baseline;
}

//! Returns the total flux of the on-pulse phase bins
Estimate<double>
Pulsar::ProfileStats::get_total (bool subtract_baseline) const try
{
  if (!built)
  {
#if _DEBUG
    cerr << "Pulsar::ProfileStats::get_total this=" << this
	 << " regions_set=" << regions_set << " calling build" << endl;
#endif
    build ();
  }

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
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::get_total";
   }

unsigned Pulsar::ProfileStats::get_onpulse_nbin () const
{
  if (!built)
    build ();
  return (unsigned) onpulse.get_weight_sum();
}

unsigned Pulsar::ProfileStats::get_baseline_nbin () const
{
  if (!built)
    build ();
  return (unsigned) baseline.get_weight_sum();
}

//! Return true if the specified phase bin is in the on pulse window
bool Pulsar::ProfileStats::get_onpulse (unsigned ibin) const try
{
  if (!built)
    build ();

  return onpulse[ibin];
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::get_onpulse (ibin)";
   }

void Pulsar::ProfileStats::set_onpulse (unsigned ibin, bool val) try
{
  if (!built)
    build ();
  onpulse[ibin] = val;
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::set_onpulse (ibin, val)";
   }

//! Return true if the specified phase bin is in the baseline window
bool Pulsar::ProfileStats::get_baseline (unsigned ibin) const try
{
  if (!built)
    build ();
  return baseline[ibin];
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::get_baseline (ibin)";
   }

Estimate<double> Pulsar::ProfileStats::get_baseline_variance () const try
{
#if _DEBUG
  cerr << "Pulsar::ProfileStats::get_baseline_variance profile=" << profile.get() 
       << " built=" << built << " variance=" << baseline_variance << endl;
#endif

  if (!built)
  {
#if _DEBUG
    cerr << "Pulsar::ProfileStats::get_baseline_variance call build" << endl;
#endif

    build ();
  }

  if (baseline_variance.get_value() == 0)
  {
#if _DEBUG
    cerr << "Pulsar::ProfileStats::get_baseline_variance get variance" << endl;
#endif

    baseline_variance = baseline.get_variance();
  }

#if _DEBUG
  cerr << "Pulsar::ProfileStats::get_baseline_variance variance=" << baseline_variance << endl;
#endif

  return baseline_variance;
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::get_baseline_variance";
   }

//! Return the on-pulse phase bin mask
Pulsar::PhaseWeight* Pulsar::ProfileStats::get_onpulse () try
{
  if (!built)
    build ();

  return &onpulse;
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::get_onpulse";
   }

//! Return the off-pulse baseline mask
Pulsar::PhaseWeight* Pulsar::ProfileStats::get_baseline () try
{
  if (!built)
    build ();
  return &baseline;
}
 catch (Error& error)
   {
     throw error += "Pulsar::ProfileStats::get_baseline";
   }

//! Return the off-pulse baseline mask
Pulsar::PhaseWeight* Pulsar::ProfileStats::get_all ()
{
  if (!built)
    build ();
  return &all;
}

void Pulsar::ProfileStats::build () const try
{
  // Profile::verbose = true;

  baseline_variance = 0.0;

  if (!profile)
  {
    if (Profile::verbose)
      cerr << "Pulsar::ProfileStats::build this=" << this
	   << " Profile not set" << endl;
    return;
  }

  if (regions_set)
  {
    if (Profile::verbose)
      cerr << "Pulsar::ProfileStats::build this=" << this
	   << " regions set" << endl;

    onpulse.set_Profile (profile);
    baseline.set_Profile (profile);
    all.set_Profile (profile);

    built = true;
    return;
  }

  if (Profile::verbose)
    cerr << "Pulsar::ProfileStats::build this=" << this
	 << " computing on-pulse and baseline of profile=" << profile.get() 
	 << endl;

  all = PhaseWeight (profile.get());

  if (include_estimator)
  {
    include_estimator->set_Profile (profile);
    include_estimator->get_weight (&include);
    onpulse_estimator->set_include (&include);
    get_baseline_estimator()->set_include (&include);

    all = include;
  }

  if (exclude_estimator)
  {
    exclude_estimator->set_Profile (profile);
    exclude_estimator->get_weight (&exclude);
    onpulse_estimator->set_exclude (&exclude);
    get_baseline_estimator()->set_exclude (&exclude);

    PhaseWeight negation = exclude;
    negation.negate();
    all *= negation;
  }

  onpulse_estimator->set_Profile (profile);
  onpulse_estimator->get_weight (&onpulse);

  get_baseline_estimator()->set_Profile (profile);
  get_baseline_estimator()->get_weight (&baseline);

  built = true;

  if (Profile::verbose)
  {
    cerr << "Pulsar::ProfileStats::build nbin=" << profile->get_nbin()
	 << " on-pulse=" << onpulse.get_weight_sum()
	 << " baseline=" << baseline.get_weight_sum() << endl;

#if _DEBUG
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
#endif
  }
}
catch (Error& error)
{
  throw error += "Pulsar::ProfileStats::build";
}

//! Set the signal-to-noise ratio estimator
void Pulsar::ProfileStats::set_snr_estimator (const std::string& name)
{
  snratio_estimator = SNRatioEstimator::factory (name);
}

//! Get the signal-to-noise ratio
double Pulsar::ProfileStats::get_snr () const
{
#if _DEBUG
  cerr << "Pulsar::ProfileStats::get_snr profile=" << profile.get() << endl;
#endif

  if (snratio_estimator)
    return snratio_estimator->get_snr ( profile );
  else
    return profile->snr();
}

TextInterface::Parser* Pulsar::ProfileStats::get_snr_interface ()
{
  if (snratio_estimator)
    return snratio_estimator->get_interface();
  else
    return StrategySet::get_default_snratio().get_value()->get_interface();
}

//! Set the pulse width estimator
void Pulsar::ProfileStats::set_pulse_width_estimator (const std::string& name)
{
  width_estimator = WidthEstimator::factory (name);
}

//! Get the pulse width
Phase::Value Pulsar::ProfileStats::get_pulse_width () const
{
  Phase::Value width = width_estimator->get_width( profile );
  return width;
}

TextInterface::Parser* Pulsar::ProfileStats::get_pulse_width_interface ()
{
  return width_estimator->get_interface();
}

#include "Pulsar/ProfileStatsInterface.h"

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ProfileStats::get_interface ()
{
  return new Interface (this);
}


