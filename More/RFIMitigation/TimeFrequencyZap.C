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

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationExpert.h"

#include "Pulsar/Profile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/ArchiveStatistic.h"
#include "Pulsar/Index.h"

#include "Pulsar/StandardOptions.h"

#include <stdio.h>
#include <assert.h>

// #define _DEBUG 1

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

  add( &TimeFrequencyZap::get_jobs,
       &TimeFrequencyZap::set_jobs,
       "jobs", "Tasks performed on clone before computing statistic" );

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

  add( &TimeFrequencyZap::get_fscrunch_factor,
       &TimeFrequencyZap::set_fscrunch_factor,
       "fscrunch", "Compute mask after fscrunch by this factor" );

  add( &TimeFrequencyZap::get_recompute,
       &TimeFrequencyZap::set_recompute,
       "recompute", "Recompute statistic on each iteration" );

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
  fscrunch_factor = 1;
  
  polns = ""; // Defaults to all

  // TODO make this a config option
  smoother = new DoubleMedian;

  // TODO this too
  masker = new SumThreshold;

  max_iterations = 1;
  recompute = false;

  nmasked = 0;
  report = false;
}

void delete_edges (Pulsar::Archive* data, unsigned delete_nchan)
{
  unsigned nchan = data->get_nchan();
  unsigned nsubint = data->get_nsubint();

  vector<unsigned> channels (delete_nchan);
  unsigned half_factor = delete_nchan/2;
  for (unsigned i=0; i<half_factor; i++)
  {
    channels[i] = nchan -i -1;
    channels[i+half_factor] = half_factor -i -1;
  }

#if _DEBUG
  cerr << "delete chans:";
  for (unsigned i=0; i<delete_nchan; i++)
    cerr << " " << channels[i];
  cerr << endl;
#endif
  
  assert( channels[delete_nchan-1] == 0 );
      
  for (unsigned isub=0; isub<nsubint; isub++)
  {
    Pulsar::Integration* subint = data->get_Integration(isub);
    for (unsigned i=0; i<channels.size(); i++)
    {
      subint->expert()->remove( channels[i] );
    }
    assert (subint->get_nchan() == nchan - delete_nchan );
  }
  data->expert()->set_nchan( nchan - delete_nchan );
}

void copy_weights (Pulsar::Archive* to, const Pulsar::Archive* from)
{
  unsigned nsubint = to->get_nsubint();
  unsigned nchan = to->get_nchan();

  assert (nsubint == from->get_nsubint());
  assert (nchan == from->get_nchan());
  
  for (unsigned isub=0; isub<nsubint; isub++)
  {
    Pulsar::Integration* sto = to->get_Integration(isub);
    const Pulsar::Integration* sfrom = from->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++)
      sto->set_weight(ichan, sfrom->get_weight(ichan));
  }
}

void Pulsar::TimeFrequencyZap::transform (Archive* archive)
{
#if _DEBUG
  cerr << "TimeFrequencyZap::transform archive=" << (void*) archive << endl;
#endif
  
  Reference::To<Archive> data = archive;
  bool cloned = false;

  unsigned initial_nonmasked = 0;

  if (report)
  {
    unsigned nsubint = data->get_nsubint();
    unsigned nchan = data->get_nchan();
  
    for (unsigned isub=0; isub<nsubint; isub++) 
    {
      Integration* subint = data->get_Integration(isub);
      for (unsigned ichan=0; ichan<nchan; ichan++) 
	if ( subint->get_weight(ichan) != 0 )
	  initial_nonmasked ++;
    }
  }
  
  if ((regions_from_total || fscrunch_factor > 1) && !data->get_dedispersed())
  {
#if _DEBUG
    cerr << "TimeFrequencyZap::transform need to dedisperse" << endl;
#endif
    // Need to make sure we are using a dedispersed version of the Archive

    /* optimization: when transform is called multiple times on the same
       archive, it can improve performance to perform this step only once.
       Therefore, save the dedispersed archive and re-use it if possible. */

    if (last_dedispersed == data)
    {
#if _DEBUG
      cerr << "TimeFrequencyZap::transform re-use dedispersed clone" << endl;
#endif
      // data weights were likely changed on last call
      copy_weights (dedispersed_clone, data);
    }
    else
    {
#if _DEBUG
      cerr << "TimeFrequencyZap::transform clone and dedisperse" << endl;
#endif
      last_dedispersed = data;

      data = archive->clone();
      cloned = true;
      data->dedisperse();
      dedispersed_clone = data;
    }
  }
  
  if (pscrunch && data->get_npol()!=1) 
  {
    if (!cloned)
      data = archive->clone();
    cloned = true;
    
    data->pscrunch();
  }

  if (jobs != "")
  {
#if _DEBUG
    cerr << "TimeFrequencyZap::transform jobs='" << jobs << "'" << endl;
#endif
    
    StandardOptions processor;
    processor.add_job (jobs);

    if (!cloned)
      data = archive->clone();
    cloned = true;
    
    processor.process ( data );
    data = processor.result();
  }

  Reference::To<Archive> backup = data;

  if (fscrunch_factor > 1)
  {
#if _DEBUG
    cerr << "TimeFrequencyZap::transform"
      " fscrunch by " << fscrunch_factor << endl;
#endif
    
    data = backup->clone();
    data->fscrunch (fscrunch_factor);
  }

#if _DEBUG
  cerr << "TimeFrequencyZap::transform performing transformation" << endl;
#endif

  unsigned total_masked = 0;

  transform (data, archive);

  // nmasked set by transform
  total_masked += nmasked;
    
  if (fscrunch_factor > 1)
  {
    // do it again with the fscrunched channel boundaries offset by 0.5*chbw
    
    data = backup->clone();

#if _DEBUG
    cerr << "TimeFrequencyZap::transform delete edges" << endl;
#endif
    
    delete_edges (data, fscrunch_factor);

#if _DEBUG
    cerr << "TimeFrequencyZap::transform"
      " fscrunch by " << fscrunch_factor << endl;
#endif
    
    data->fscrunch (fscrunch_factor);

#if _DEBUG
    cerr << "TimeFrequencyZap::transform performing transformation" << endl;
#endif
    
    transform (data, archive, fscrunch_factor / 2);

    // nmasked set by transform
    total_masked += nmasked;
  }

  if (report)
  {
    string ret;
    
    if (statistic)
      ret += "stat=" + statistic->get_identity();
    else
      ret += "exp=" + expression;

    ret += " tested=" + tostring(initial_nonmasked)
      + " masked=" + tostring(total_masked) 
      + " %=" + tostring((total_masked)*100.0/initial_nonmasked);

    cout << ret << endl;
  }
}

void Pulsar::TimeFrequencyZap::transform (Archive* data, Archive* archive,
					  unsigned chan_offset)
{
  // Size arrays
  nchan = data->get_nchan();
  nsubint = data->get_nsubint();

  TextInterface::parse_indeces(pol_i, polns, data->get_npol());
  //npol = data->get_npol();
  npol = pol_i.size();

#if _DEBUG
  cerr << "TimeFrequencyZap::transform nsubint=" << nsubint
       << " nchan=" << nchan << " npol=" << npol << endl;
#endif
  
  freq.resize(nchan*nsubint);
  time.resize(nsubint);
  stat.resize(nchan*nsubint*npol);
  mask.resize(nchan*nsubint);
  MJD ep0 = data->get_Integration(0)->get_epoch();

  // Fill freq and mask arrays from data, init stat to 0
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

  unsigned iter = 0;
  unsigned nmasked_during_loop = 0;
  
  do 
  {
#if _DEBUG
    cerr << "TimeFrequencyZap::transform iteration=" << iter << endl;
#endif

    if (iter == 0 || recompute)
    {
      if (iter > 0)
      {
	apply_mask (data);
	if (data == archive)
	  nmasked_during_loop += nmasked;
      }
      
      compute_stat(data);
    }
    
    // sets nmasked
    update_mask();

    iter ++;
  }
  while (iter < max_iterations && nmasked > 0);

  apply_mask (archive, fscrunch_factor, chan_offset);

  nmasked += nmasked_during_loop;

#if _DEBUG
  cerr << "TimeFrequencyZap::transform nmasked=" << nmasked << endl;
#endif
}



void Pulsar::TimeFrequencyZap::compute_stat (Archive* data)
{
#if _DEBUG
  cerr << "TimeFrequencyZap::compute_stat nsubint=" << nsubint
       << " nchan=" << nchan << endl;
#endif

  assert (data->get_nsubint() == nsubint);
  assert (data->get_nchan() == nchan);
  
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
  {
    // force recomputation of any buffered data
    statistic->set_Archive (NULL);
    statistic->set_Archive (data);
  }
 
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

      if (statistic)
	statistic->set_chan (ichan);
    
      for (unsigned ipol=0; ipol<npol; ipol++)
      {
        float fval = 0;
        if (statistic)
        {
	  statistic->set_pol (pol_i[ipol]);
          fval = statistic->get();
        }
        else
        {
	  Reference::To<const Profile> prof;
	  prof = subint->get_Profile(pol_i[ipol],ichan);

          stats->set_Profile(prof);
          string val = process(parser,expression);
          fval = fromstring<float>(val);
        }

#if _DEBUG
	cerr << "isub=" << isub << " ichan=" << ichan << " ipol=" << ipol
	     << " fval=" << fval << endl;
#endif
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

void Pulsar::TimeFrequencyZap::apply_mask (Archive* archive,
					   unsigned fscrunch_factor,
					   unsigned chan_offset)
{
#if _DEBUG
  cerr << "TimeFrequencyZap::apply_mask archive=" << (void*) archive
       << " fscrunch=" << fscrunch_factor << " chan_offset=" << chan_offset
       << endl;
#endif
  
  // count weights changed to zero
  nmasked = 0;

  assert ( nchan * fscrunch_factor + chan_offset <= archive->get_nchan() );
  
  // apply mask back to original archive
  for (unsigned isub=0; isub<nsubint; isub++) 
  {
    Integration* subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++) 
    {
      float wt = mask[idx(isub,ichan)];
      if (fscrunch_factor > 1 && wt != 0.0)
      {
	// change the weights of the original archive only if setting to zero
	// so that the weights are not set to that of the fscrunched data
	continue;
      }
      for (unsigned jchan=0; jchan < fscrunch_factor; jchan++)
      {
	unsigned ch = ichan*fscrunch_factor + jchan + chan_offset;
	if (wt == 0 && subint->get_weight(ch) != 0)
	  nmasked ++;
	subint->set_weight(ch, wt);
      }
    }
  }
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
