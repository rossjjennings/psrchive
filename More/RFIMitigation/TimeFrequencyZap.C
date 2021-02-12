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
#include "Pulsar/ArchiveStatistic.h"
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
       &ArchiveStatistic::get_interface,
       "stat", "Statistic computed for each subint/chan" );

  add( &TimeFrequencyZap::get_smoother,
       &TimeFrequencyZap::set_smoother,
       &TimeFrequencySmooth::get_interface,
       "smooth", "Algorithm used to smooth/detrend statistics" );
    
  add( &TimeFrequencyZap::get_masker,
       &TimeFrequencyZap::set_masker,
       &TimeFrequencyMask::get_interface,
       "mask", "Algorithm used to mask subints/channels" );
    
  add( &TimeFrequencyZap::get_cutoff_threshold,
       &TimeFrequencyZap::set_cutoff_threshold,
       "cutoff", "Outlier threshold (# sigma)" );

  add( &TimeFrequencyZap::get_max_iterations,
       &TimeFrequencyZap::set_max_iterations,
       "iterations", "Maximum number of times to iterate" );

  add( &TimeFrequencyZap::get_polarizations,
       &TimeFrequencyZap::set_polarizations,
       "pols", "Polarizations to analyze" );
  
  add( &TimeFrequencyZap::get_report,
       &TimeFrequencyZap::set_report,
       "report", "Print one-line report to stdout" );
}

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::TimeFrequencyZap::TimeFrequencyZap ()
{
  expression = "off:rms";
  regions_from_total = true;
  pscrunch = false;
  polns = ""; // Defaults to all

  // TODO make this a config option
  smoother = new DoubleMedian;

  // TODO this too
  masker = new SumThreshold;

  max_iterations = 1;
  nmasked = 0;
  report = false;
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
  TextInterface::parse_indeces(pol_i, polns, data->get_npol());
  //npol = data->get_npol();
  npol = pol_i.size();

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

  unsigned iter = 0;
  unsigned total_masked = 0;

  // kludge to get while loop started
  nmasked = 1;
  
  while (iter < max_iterations && nmasked > 0)
  {
    update_mask();

    // nmasked set by update_mask
    total_masked += nmasked;
    iter ++;
  }

  if (report)
  {
    string ret;
    
    if (statistic)
      ret += "stat=" + statistic->get_identity();
    else
      ret += "exp=" + expression;

    ret += " tested=" + tostring(nonmasked)
      + " iter=" + tostring(iter)
      + " masked=" + tostring(total_masked) 
      + " %=" + tostring((total_masked)*100.0/nonmasked);

    cout << ret << endl;
  }

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
  nonmasked = 0;
  
  // Only use ProfileStats here.  More specialized things could be 
  // implemented in derived classes.
  Reference::To<ProfileStats> stats;
  Reference::To<TextInterface::Parser> parser;

  if (expression != "")
  {
    stats = new ProfileStats;
    parser = stats->get_interface();
    parser->set_prefix_name(false);

    if (regions_from_total) 
      stats->select_profile(data->total()->get_Profile(0,0,0));
  }

  if (statistic)
    statistic->set_Archive (data);
  
  // Eval expression, fill stats array
  for (unsigned isub=0; isub<nsubint; isub++) 
  {
    Integration* subint = data->get_Integration(isub);

    if (statistic)
      statistic->set_subint (isub);
    
    for (unsigned ichan=0; ichan<nchan; ichan++) 
    {
      if (mask[idx(isub,ichan)]==0.0)
        continue;
      else
	nonmasked ++;
      
      if (statistic)
	statistic->set_chan (ichan);
    
      for (unsigned ipol=0; ipol<npol; ipol++)
      {
        Reference::To<const Profile> prof = subint->get_Profile(pol_i[ipol],ichan);

        float fval = 0;
        if (statistic)
        {
	  statistic->set_pol (pol_i[ipol]);
          fval = statistic->get();
        }
        else
        {
          stats->set_Profile(prof);
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

  if (smoother)
    smoother->smooth(smoothed, stat, mask, freq, time);

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

  nmasked = masker->update_mask(mask, stat, smoothed, nsubint, nchan, npol);
}

//! Set the statistic
void Pulsar::TimeFrequencyZap::set_statistic (ArchiveStatistic* stat)
{
  statistic = stat;
  expression = "";
  regions_from_total = false;
}

void Pulsar::TimeFrequencyZap::set_expression (const std::string& exp)
{
  expression = exp;
  statistic = 0;
}

//! Get the statistic
Pulsar::ArchiveStatistic* Pulsar::TimeFrequencyZap::get_statistic () const
{
  return statistic;
}

//! Set the smoother
void Pulsar::TimeFrequencyZap::set_smoother (TimeFrequencySmooth* smooth)
{
  smoother = smooth;
}

//! Get the smoother
Pulsar::TimeFrequencySmooth* Pulsar::TimeFrequencyZap::get_smoother () const
{
  return smoother;
}

//! Set the masker
void Pulsar::TimeFrequencyZap::set_masker (TimeFrequencyMask* mask)
{
  masker = mask;
}

//! Get the masker
Pulsar::TimeFrequencyMask* Pulsar::TimeFrequencyZap::get_masker () const
{
  return masker;
}
