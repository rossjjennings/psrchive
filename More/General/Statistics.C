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
  set_Archive (data);
}

//! Get the text interface to this
TextInterface::Parser* Pulsar::Statistics::get_interface ()
{
  return new Interface (this);
}

Pulsar::StrategySet* Pulsar::Statistics::get_strategy () const try
{
  return get_Archive()->get_strategy();
}
catch (Error& error)
{
  throw error += "Pulsar::Statistics::get_strategy";
}

Phase::Value Pulsar::Statistics::get_peak () const
{
  Phase::Value width = get_Profile()->find_max_phase();
  set_period (width);
  return width;
}

void Pulsar::Statistics::set_period (Phase::HasUnit& value) const
{
  // set the period in milliseconds
  value.set_period( get_Integration()->get_folding_period() * 1e3 );
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
  return get_stats()->get_baseline();
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_baseline";
   }

//! Get the on-pulse phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_onpulse () try
{
  return get_stats()->get_onpulse();
}
 catch (Error& error)
   {
     throw error += "Pulsar::Statistics::get_onpulse";
   }

//! Get all phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_all () try
{
  return get_stats()->get_all();
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
  if (archive)
    stats = archive->get_strategy()->get_stats();

  // avoid recursion - part 2
  // (Plugin::setup might call a function that calls setup_stats)
  if (is_current())
    return;

  // avoid recursion - part 1
  /*
     Plugin::setup might call a function that calls setup_stats
     Calling HasArchive::get_Profile makes HasArchive::is_current return true
  */
  Reference::To<const Profile> tmp = get_Profile();

  for (unsigned i=0; i<plugins.size(); i++)
    plugins[i]->set_setup (false);

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats stats=" << stats.ptr() << endl;

  stats->set_profile( get_Profile() );

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats profile=" << profile.ptr() << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::Statistics::setup_stats";
}

void Pulsar::Statistics::add_plugin (Plugin* plugin)
{
  plugin->parent = this;
  plugins.push_back(plugin);

  // Calling HasArchive::set_Archive makes HasArchive::is_current return false
  if (archive)
    set_Archive(archive);
}

