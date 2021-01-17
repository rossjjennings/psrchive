/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeFrequencyZap.h"
#include "Pulsar/TimeFrequencySmooth.h"
#include "Pulsar/TimeFrequencyMask.h"
#include "Pulsar/DoubleMedian.h"
#include "Pulsar/SumThreshold.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/ProfileStatistic.h"
#include "Pulsar/Index.h"

#include <stdio.h>

using namespace std;

// Text interface to TimeFrequencyZap
TextInterface::Parser* Pulsar::TimeFrequencyZap::get_interface ()
{
  return new Interface (this);
}

Pulsar::TimeFrequencyZap::Interface::Interface (TimeFrequencyZap* instance)
{
  if (instance)
    set_instance (instance);

  add( &TimeFrequencyZap::get_expression,
       &TimeFrequencyZap::set_expression,
       "exp", "Statistical expression" );

  add( &TimeFrequencyZap::get_statistic,
       &TimeFrequencyZap::set_statistic,
       "stat", "Profile statistic" );

  add( &TimeFrequencyZap::get_cutoff_threshold,
       &TimeFrequencyZap::set_cutoff_threshold,
       "cutoff", "Outlier threshold (# sigma)" );
}

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::TimeFrequencyZap::TimeFrequencyZap ()
{
  expression = "off:rms";
  regions_from_total = true;
  pscrunch = false;

  // TODO make this a config option
  smoother = new DoubleMedian;

  // TODO this too
  masker = new SumThreshold;
}

void Pulsar::TimeFrequencyZap::transform (Archive* archive)
{
  data = archive;

  if (regions_from_total && !data->get_dedispersed())
  {
    // Need to make sure we are using a dedispersed version of the Archive
    data = data->clone();
    data->dedisperse();
  }

  if (pscrunch && data->get_npol()!=1) 
  {
    data = data->clone();
    data->pscrunch();
  }

  // Size arrays
  nchan = data->get_nchan();
  nsubint = data->get_nsubint();
  npol = data->get_npol();
  freq.resize(nchan*nsubint);
  time.resize(nsubint);
  stat.resize(nchan*nsubint*npol);
  mask.resize(nchan*nsubint);
  MJD ep0 = data->get_Integration(0)->get_epoch();

  // Fill freq and mask arrays from archive, init stat to 0
  for (unsigned isub=0; isub<nsubint; isub++) 
  {
    Integration* subint = data->get_Integration(isub);
    time[isub] = (subint->get_epoch() - ep0).in_seconds();
    for (unsigned ichan=0; ichan<nchan; ichan++) 
    {
      freq[idx(isub,ichan)] = subint->get_centre_frequency(ichan);
      mask[idx(isub,ichan)] = subint->get_weight(ichan);
      for (unsigned ipol=0; ipol<npol; ipol++) 
      {
        stat[idx(isub,ichan,ipol)] = 0.0;
      }
    }
  }

  compute_stat();

  update_mask();

  // apply mask back to original archive
  for (unsigned isub=0; isub<nsubint; isub++) 
  {
    Integration* subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++) 
    {
      subint->set_weight(ichan, mask[idx(isub,ichan)]);
    }
  }
}

void Pulsar::TimeFrequencyZap::compute_stat ()
{
  // Only use ProfileStats here.  More specialized things could be 
  // implemented in derived classes.
  ProfileStats stats;
  Reference::To<TextInterface::Parser> parser = stats.get_interface();
  parser->set_prefix_name(false);

  if (regions_from_total) 
  {
    stats.select_profile(data->total()->get_Profile(0,0,0));
  }

  // Eval expression, fill stats array
  for (unsigned isub=0; isub<nsubint; isub++) 
  {
    Integration* subint = data->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++) 
    {
      if (mask[idx(isub,ichan)]==0.0)
        continue;

      for (unsigned ipol=0; ipol<npol; ipol++)
      {
        Reference::To<const Profile> prof = subint->get_Profile(ipol,ichan);

        float fval = 0;
        if (statistic)
        {
          fval = statistic->get(prof);
        }
        else
        {
          stats.set_Profile(prof);
          string val = process(parser,expression);
          fval = fromstring<float>(val);
        }
        stat[idx(isub,ichan,ipol)] = fval;
      }
    }
  }
}

void Pulsar::TimeFrequencyZap::update_mask ()
{
  std::vector<float> smoothed;
  smoother->smooth(smoothed, stat, mask, freq, time);
  // const unsigned ntot = nsubint * nchan * npol;

#if 0 
  // Output arrays for debug
  FILE *s1, *s2;
  s1 = fopen("stat.dat","w");
  s2 = fopen("smoothed.dat","w");
  for (unsigned i=0; i<ntot; i++) {
    float ftmp = stat[i];
    fwrite(&ftmp, sizeof(float), 1, s1);
    ftmp = smoothed[i];
    fwrite(&ftmp, sizeof(float), 1, s2);
  }
  fclose(s1);
  fclose(s2);
#endif

  masker->update_mask(mask, stat, smoothed, nsubint, nchan, npol);
}

//! Set the profile statistic
void Pulsar::TimeFrequencyZap::set_statistic (const std::string& name)
{
  statistic = ProfileStatistic::factory (name);
  regions_from_total = false;
}

//! Get the profile statistic
std::string Pulsar::TimeFrequencyZap::get_statistic () const
{
  if (!statistic)
    return "none";

  return statistic->get_identity();
}

