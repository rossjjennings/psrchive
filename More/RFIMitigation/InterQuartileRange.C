/***************************************************************************
 *
 *   Copyright (C) 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InterQuartileRange.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/ArchiveStatistic.h"
#include "Pulsar/Index.h"

#include <fstream>
#include <algorithm>
#include <assert.h>

using namespace std;

// #define _DEBUG 1

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::InterQuartileRange::InterQuartileRange ()
{
  cutoff_threshold_max = cutoff_threshold_min = 1.5;
  max_iterations = 30;
  logarithmic = false;

  median_nchan = median_nsubint = 0;
  median_filename = "test_log_range.txt";
}

Pulsar::InterQuartileRange::~InterQuartileRange ()
{
}

void Pulsar::InterQuartileRange::transform (Archive* archive)
{
  tot_valid = 0;
  tot_high = 0;
  tot_low = 0;
  iter = 0;

  if (Archive::verbose > 1)
    cerr << "InterQuartileRange::transform archive=" << archive << endl;

  compute (archive);
  
  while (iter < max_iterations)
  {
#if _DEBUG
    cerr << "InterQuartileRange::transform iteration=" << iter << endl;
#endif
    once ();

    if (iter == 0)
      tot_valid = valid;

    tot_high += too_high;
    tot_low += too_low;
    iter ++;
    
    if (too_high + too_low == 0)
      break;
  }

  mask (archive);
}

std::string Pulsar::InterQuartileRange::get_report () const
{
#if _DEBUG
  cerr << "Pulsar::InterQuartileRange::get_report" << endl;
#endif

  string ret;
  if (statistic)
    ret += "stat=" + statistic->get_identity() + " ";

  ret += "tested=" + tostring(tot_valid) + " iter=" + tostring(iter)
       + " high=" + tostring(tot_high) + " low=" + tostring(tot_low)
       + " %=" + tostring((tot_high+tot_low)*100.0/tot_valid);

  return ret;
}

void Pulsar::InterQuartileRange::compute (Archive* archive)
{
  Reference::To<ProfileStats> stats;
  Reference::To<TextInterface::Parser> parser;

  if (!expression.empty())
  {
    stats = new ProfileStats;
    parser = stats->get_interface ();
  }

  if (statistic)
    statistic->set_Archive( archive );

  nchan = archive->get_nchan();
  nsubint = archive->get_nsubint();
  values.resize (nchan * nsubint);
  
  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
#if _DEBUG
    cerr << "InterQuartileRange::compute isubint=" << isubint << endl;
#endif

    if (statistic)
      statistic->set_subint ( isubint );

    Integration* subint = archive->get_Integration( isubint );
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned index = isubint * nchan + ichan;

      values[index].second = subint->get_weight(ichan);

      if (values[index].second == 0)
	continue;

      Index pol (0, true); // integrate over polarizations
      Reference::To<const Profile> profile;

      float value = 0;

      if (statistic)
      {
        statistic->set_chan( ichan );
        statistic->set_pol( pol );
        value = statistic->get();
      }
      else if (stats)
      {
        profile = Pulsar::get_Profile (subint, pol, ichan);

	stats->set_Profile (profile);
	string value = process( parser, expression );
	value = fromstring<float>( value );
      }
      else
      {
	double mean = 0;
	double var = 0;
	profile->stats (&mean, &var);
	value = sqrt(var)/mean; // i.e. the modulation index
      }

      if (logarithmic)
        value = log10(value);
      
      values[index].first = value;
    }
  }
}

void Pulsar::InterQuartileRange::once ()
{
  too_high = 0;
  too_low = 0;
  valid = 0;

  if (median_nchan > 2 || median_nsubint > 2)
    compute_median ();

  std::vector<float> data (nchan * nsubint, 0.0);

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned index = isubint * nchan + ichan;

      if (values[index].second == 0)
        continue;

      data[valid] = values[index].first;

      if (local_median.size() == values.size())
        data[valid] -= local_median[index];

      valid ++;
    }
  }

#ifdef _DEBUG
  cerr << "nchan=" << nchan << " nsub=" << nsubint 
       << " nchan*nsub=" << nchan*nsubint << " valid=" << valid << endl;
#endif

  if (valid < 4)
  {
#ifdef _DEBUG
    cerr << "valid < 4 - giving up" << endl;
#endif
    return;
  }

  data.resize(valid);
    
  unsigned iq1 = valid/4;
  unsigned iq3 = (valid*3)/4;

#ifdef _DEBUG
  cerr << "iQ1=" << iq1 << " iQ3=" << iq3 << endl;
#endif

  std::sort (data.begin(), data.begin()+valid);
  double Q1 = data[ iq1 ];
  double Q3 = data[ iq3 ];

  double IQR = Q3 - Q1;

#ifdef _DEBUG
  cerr << "Q1=" << Q1 << " Q3=" << Q3 << " IQR=" << IQR << endl;
#endif

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned index = isubint * nchan + ichan;

      if (values[index].second == 0)
        continue;

      float value = values[index].first;

      if (local_median.size() == values.size())
        value -= local_median[index];

      bool zap = false;

      if (cutoff_threshold_min > 0 &&
          value < Q1 - cutoff_threshold_min * IQR)
      {
#ifdef _DEBUG
  cerr << "TOO LOW ichan=" << ichan << endl;
#endif

	zap = true;
        too_low ++;
      }

      if (cutoff_threshold_max > 0 &&
          value > Q3 + cutoff_threshold_max * IQR)
      {
#ifdef _DEBUG
  cerr << "TOO HIGH ichan=" << ichan << endl;
#endif

	zap = true;
        too_high ++;
      }

      if (zap)
        values[index].second = 0;
    }
  }

#ifdef _DEBUG
  cerr << "too high=" << too_high << " too low=" << too_low << endl;
#endif
}

void Pulsar::InterQuartileRange::compute_median ()
{
  local_median.resize (nchan * nsubint);

  std::vector<float> data (median_nchan * median_nsubint, 0.0);

  unsigned half_nchan = median_nchan / 2;
  unsigned half_nsubint = median_nsubint / 2;

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned index = isubint * nchan + ichan;

      if (values[index].second == 0)
        continue;

      unsigned start_subint = 0;
      if (isubint > half_nsubint)
        start_subint = isubint - half_nsubint;
      unsigned end_subint = start_subint + median_nsubint;
      if (end_subint > nsubint)
      {
        end_subint = nsubint;
        start_subint = nsubint - median_nsubint;
      }

      unsigned start_chan = 0;
      if (ichan > half_nchan)
        start_chan = ichan - half_nchan;
      unsigned end_chan = start_chan +  median_nchan;
      if (end_chan > nchan)
      {
        end_chan = nchan;
        start_chan = nchan - median_nchan;
      }

      unsigned valid = 0;

#if _DEBUG
      cerr << "isubint=" << isubint << " ichan=" << ichan 
           << " ssub=" << start_subint << " esub=" << end_subint
           << " sch=" << start_chan << " ech=" << end_chan << endl;
#endif

      for (unsigned jsubint=start_subint; jsubint < end_subint; jsubint++)
      {
        for (unsigned jchan=start_chan; jchan < end_chan; jchan++)
        {
          unsigned jndex = jsubint * nchan + jchan;
    
          if (values[jndex].second == 0)
            continue;

          data[valid] = values[jndex].first;
          valid ++;
        }
      }

      if (valid < 2)
      {
#ifdef _DEBUG
        cerr << "valid=" << valid << " med=" << data[index] << endl;
#endif
        local_median[index] = data[index];
      }
      else
      {
        unsigned mid = valid / 2;
        std::nth_element( data.begin(), data.begin()+mid, data.begin()+valid );
        local_median[index] = data[mid];
      }
    }
  }

  if (median_filename.empty())
    return;

  ofstream out (median_filename.c_str());
  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned index = isubint * nchan + ichan;

      if (values[index].second == 0)
        continue;

      out << isubint << " " << ichan << " " << values[index].first
          << " " << local_median[index] << endl;
    }
    out << endl;
  }

  median_filename = "";
}

void Pulsar::InterQuartileRange::mask (Archive* archive)
{ 
  assert (nchan == archive->get_nchan());
  assert (nsubint == archive->get_nsubint());
  assert (values.size() == nchan * nsubint);

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    Integration* subint = archive->get_Integration( isubint );

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned index = isubint * nchan + ichan;

      if (values[index].second == 0)
        subint->set_weight (ichan, 0.0);
    }
  }
}

//! Set the profile statistic
void Pulsar::InterQuartileRange::set_statistic (const std::string& name)
{
  statistic = ArchiveStatistic::factory (name);
}

//! Get the profile statistic
std::string Pulsar::InterQuartileRange::get_statistic () const
{
  if (!statistic)
    return "none";

  return statistic->get_identity();
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::InterQuartileRange::get_interface ()
{
  return new Interface (this);
}

Pulsar::InterQuartileRange::Interface::Interface (InterQuartileRange* instance)
{
  if (instance)
    set_instance (instance);

  add( &InterQuartileRange::get_expression,
       &InterQuartileRange::set_expression,
       "exp", "Statistical expression" );

  add( &InterQuartileRange::get_statistic,
       &InterQuartileRange::set_statistic,
       "stat", "Profile statistic" );

  add( &InterQuartileRange::get_median_nchan,
       &InterQuartileRange::set_median_nchan,
       "medchan", "Median smooth over number of frequency channels");

  add( &InterQuartileRange::get_median_nsubint,
       &InterQuartileRange::set_median_nsubint,
       "medsubint", "Median smooth over number of sub-integrations");

  add( &InterQuartileRange::get_logarithmic,
       &InterQuartileRange::set_logarithmic,
       "log", "Logarithmic scale" );

  add( &InterQuartileRange::get_cutoff_threshold,
       &InterQuartileRange::set_cutoff_threshold,
       "cutoff", "Outlier threshold: Q1-cutoff*IQR - Q3+cutoff*IQR" );

  add( &InterQuartileRange::get_cutoff_threshold_min,
       &InterQuartileRange::set_cutoff_threshold_min,
       "cutmin", "Outlier threshold: Q1-cutmin*IQR - Q3+cutmax*IQR" );

  add( &InterQuartileRange::get_cutoff_threshold_max,
       &InterQuartileRange::set_cutoff_threshold_max,
       "cutmax", "Outlier threshold: Q1-cutmin*IQR - Q3+cutmax*IQR" );
}

