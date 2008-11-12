/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Statistics.h"

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
unsigned 
Pulsar::Statistics::get_cal_ntrans () const
{
  SquareWave wave;
  return wave.count_transitions (get_Profile());
}

//! Get the two bit distortion (or distance from theory)
double
Pulsar::Statistics::get_2bit_dist () const
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

