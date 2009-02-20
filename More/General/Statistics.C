/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Statistics.h"
#include "Pulsar/ProfileStats.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/TwoBitStats.h"
#include "Pulsar/NoiseStatistics.h"
#include "Pulsar/SquareWave.h"
#include "Pulsar/Profile.h"

using namespace std;


//! Default constructor
Pulsar::Statistics::Statistics (const Archive* data)
{
  if (data)
    set_Archive (data);
}

//! Set the instance from which statistics will be drawn
void Pulsar::Statistics::set_Archive (const Archive* data)
{
  archive = data;
}

const Pulsar::Profile* Pulsar::Statistics::get_Profile () const
{
  profile = Pulsar::get_Profile (archive, isubint, ipol, ichan);
  return profile;
}

//! Get the signal-to-noise ratio
double Pulsar::Statistics::get_snr () const
{
  return get_Profile()->snr();
}

//! Get the Fourier-noise-to-noise ratio
double Pulsar::Statistics::get_nfnr () const
{
  NoiseStatistics noise;
  if (archive->type_is_cal())
    noise.set_baseline_time (0.4);
  return noise.get_nfnr (get_Profile());
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

//! Get the two bit distortion (or distance from theory)
double Pulsar::Statistics::get_pcm_good () const
{
  const PolnCalibratorExtension* ext = archive->get<PolnCalibratorExtension>();
  if (!ext)
    return 0;

  if (!ext->get_has_solver())
    return 0;

  unsigned nchan = ext->get_nchan();

  double chisq = 0.0;
  unsigned nfree = 0;
  const PolnCalibratorExtension::Transformation* xform = 0;

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    xform = ext->get_transformation(ichan);
    if (xform->get_valid())
    {
      chisq += xform->get_chisq();
      nfree += xform->get_nfree();
    }
  }

  if (nfree == 0)
    nfree = 1;

  return chisq/nfree;
}

//! Get the off-pulse baseline
Pulsar::PhaseWeight* Pulsar::Statistics::get_baseline ()
{
  setup_stats ();
  return stats->get_baseline();
}

//! Get the on-pulse phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_onpulse ()
{
  setup_stats ();
  return stats->get_on_pulse();
}

//! Get all phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_all ()
{
  setup_stats ();
  return stats->get_all();
}

void Pulsar::Statistics::setup_stats ()
{
  if (!stats)
    stats = new ProfileStats;

  stats->set_profile( get_Profile() );
}

