/***************************************************************************
 *
 *   Copyright (C) 2008 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Statistics.h"
#include "Pulsar/StatisticsInterface.h"

#include "Pulsar/ProfileStats.h"
#include "Pulsar/ProfileShiftFit.h"
#include "Pulsar/ProfileStrategies.h"

#include "Pulsar/SNRatioEstimator.h"
#include "Pulsar/PhaseWidth.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/TwoBitStats.h"
#include "Pulsar/NoiseStatistics.h"
#include "Pulsar/SquareWave.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Physical.h"

using namespace std;


//! Default constructor
Pulsar::Statistics::Statistics (const Archive* data)
{
  pulse_width_estimator = new PhaseWidth;
  set_Archive (data);
}

//! Get the text interface to this
TextInterface::Parser* Pulsar::Statistics::get_interface ()
{
  return new Interface (this);
}

//! Set the instance from which statistics will be drawn
void Pulsar::Statistics::set_Archive (const Archive* data)
{
  archive = data;
  stats_setup = false;
}

const Pulsar::Archive* Pulsar::Statistics::get_Archive () const
{
  return archive;
}

const Pulsar::Integration* Pulsar::Statistics::get_Integration () const
{
  integration = Pulsar::get_Integration (archive, isubint);
  return integration;
}

const Pulsar::Profile* Pulsar::Statistics::get_Profile () const
{
  profile = Pulsar::get_Profile (get_Integration(), ipol, ichan);
  return profile;
}

//! Set the sub-integration from which statistics will be drawn
void Pulsar::Statistics::set_subint (Index _isubint)
{
  isubint = _isubint;
  stats_setup = false;
}

Pulsar::Index Pulsar::Statistics::get_subint () const
{
  return isubint;
}
    
//! Set the frequency channel from which statistics will be drawn
void Pulsar::Statistics::set_chan (Index _ichan)
{
  ichan = _ichan;
  stats_setup = false;
}

Pulsar::Index Pulsar::Statistics::get_chan () const
{
  return ichan;
}

//! Set the polarization to plot
void Pulsar::Statistics::set_pol (Index _ipol)
{
  ipol = _ipol;
  stats_setup = false;
}

Pulsar::Index Pulsar::Statistics::get_pol () const
{
  return ipol;
}

Pulsar::StrategySet* Pulsar::Statistics::get_strategy () const try
{
    return get_Archive()->get_strategy();
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_strategy";
   }

//! Set the signal-to-noise ratio estimator
void Pulsar::Statistics::set_snr_estimator (const std::string& name)
{
  snr_estimator = SNRatioEstimator::factory (name);
}

//! Get the signal-to-noise ratio
double Pulsar::Statistics::get_snr () const
{
  if (snr_estimator)
    return snr_estimator->get_snr ( get_Profile() );
  else
    return get_Profile()->snr();
}

TextInterface::Parser* Pulsar::Statistics::get_snr_interface ()
{
  if (snr_estimator)
    return snr_estimator->get_interface();
  else
    return StrategySet::default_snratio.get_value()->get_interface();
}

Phase::Value Pulsar::Statistics::get_peak () const
{
  Phase::Value width = get_Profile()->find_max_phase();
  set_period (width);
  return width;
}

//! Set the pulse width estimator
void Pulsar::Statistics::set_pulse_width_estimator (const std::string& name)
{
  pulse_width_estimator = WidthEstimator::factory (name);
}

void Pulsar::Statistics::set_period (Phase::HasUnit& value) const
{
  // set the period in milliseconds
  value.set_period( get_Integration()->get_folding_period() * 1e3 );
}

//! Get the pulse width
Phase::Value Pulsar::Statistics::get_pulse_width () const
{
  Phase::Value width = pulse_width_estimator->get_width( get_Profile() );
  set_period (width);
  return width;
}

TextInterface::Parser* Pulsar::Statistics::get_pulse_width_interface ()
{
  return pulse_width_estimator->get_interface();
}

//! Get the Fourier-noise-to-noise ratio
double Pulsar::Statistics::get_nfnr () const
{
  NoiseStatistics noise;
  if (archive->type_is_cal())
    noise.set_baseline_time (0.4);
  return noise.get_nfnr (get_Profile());
}

double Pulsar::Statistics::get_effective_duty_cycle () const
{
  ProfileShiftFit fit;
  fit.choose_maximum_harmonic = true;
  fit.set_standard( get_Profile() );
  return fit.get_effective_duty_cycle ();
}

//! Get the number of cal transitions
unsigned Pulsar::Statistics::get_cal_ntrans () const
{
  SquareWave wave;
  return wave.count_transitions (get_Profile());
}

//! Get the two bit distortion (or distance from theory)
double Pulsar::Statistics::get_2bit_dist () const
{
  const TwoBitStats* tbs = archive->get<TwoBitStats>();
  if (!tbs)
    return 0;

  double distortion = 0.0;
  unsigned ndig=tbs->get_ndig();
  for (unsigned idig=0; idig < ndig; idig++)
    distortion += tbs->get_distortion(idig);
  return distortion;
}

//! Get the off-pulse baseline
Pulsar::PhaseWeight* Pulsar::Statistics::get_baseline () try
{
  setup_stats ();
  return stats->get_baseline();
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_baseline";
   }

//! Get the on-pulse phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_onpulse () try
{
  setup_stats ();
  return stats->get_onpulse();
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_onpulse";
   }

//! Get all phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_all () try
{
  setup_stats ();
  return stats->get_all();
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_all";
   }

//! Get the statistics calculator
Pulsar::ProfileStats* Pulsar::Statistics::get_stats () try
{
  setup_stats ();
  return stats;
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_stats";
   }

//! Get the weighted frequency of the Pulsar::Archive
double Pulsar::Statistics::get_weighted_frequency () const
{
  integration = Pulsar::get_Integration (archive, 0);
  return integration->weighted_frequency (0, archive->get_nchan());
}

double Pulsar::Statistics::get_bin_width () const
{
  integration = Pulsar::get_Integration (archive, 0);
  return integration->get_folding_period() / integration->get_nbin();
}

double Pulsar::Statistics::get_dispersive_smearing () const
{
  double dm        = archive->get_dispersion_measure();
  double freq      = archive->get_centre_frequency();
  double bw        = fabs(archive->get_bandwidth());
  double chan_bw   = bw / archive->get_nchan();

  freq -= 0.5 * (bw - chan_bw);

  /*
    cerr << "Frequency = " << freq << endl;
    cerr << "Channel bandwidth = " << chan_bw << endl;
    cerr << "DM = " << dm << endl;
  */

  // DM smearing in seconds
  return dispersion_smear (dm, freq, chan_bw);
}

void Pulsar::Statistics::setup_stats () try
{
  if (!stats)
  {
    stats = new ProfileStats;
    stats_setup = false;
  }

  // avoid recursion - part 2
  // (Plugin::setup might call a function that calls setup_stats)
  if (stats_setup)
    return;

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats stats=" << stats.ptr() << endl;

  stats->set_profile( get_Profile() );

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats profile=" << profile.ptr() << endl;

  // avoid recursion - part 1
  // (Plugin::setup might call a function that calls setup_stats)
  stats_setup = true;

  for (unsigned i=0; i<plugins.size(); i++)
    plugins[i]->setup ();

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats done" << endl;
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::setup_stats";
   }

void Pulsar::Statistics::add_plugin (Plugin* plugin)
{
  plugin->parent = this;
  plugins.push_back(plugin);
  stats_setup = false;
}

