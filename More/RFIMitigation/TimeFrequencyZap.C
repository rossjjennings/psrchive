/***************************************************************************
 *
 *   Copyright (C) 2018 - 2021 by Paul Demorest and Willem van Straten
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
#include "Pulsar/ArchiveComparisons.h"
#include "Pulsar/Index.h"

#include "Pulsar/StandardOptions.h"

#include <stdio.h>
#include <assert.h>

// #define _DEBUG 1
#include "debug.h"

using namespace std;
using namespace Pulsar;

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

  add( &TimeFrequencyZap::get_logarithmic,
       &TimeFrequencyZap::set_logarithmic,
       "log", "Use the logarithm of the statistic" );

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

  add( &TimeFrequencyZap::get_fscrunch,
       &TimeFrequencyZap::set_fscrunch,
       "fscrunch", "Compute mask after fscrunch" );

  add( &TimeFrequencyZap::get_bscrunch,
       &TimeFrequencyZap::set_bscrunch,
       "bscrunch", "Compute covariance matrix after bscrunch" );

  add( &TimeFrequencyZap::get_recompute,
       &TimeFrequencyZap::set_recompute,
       "recompute", "Recompute statistic on each iteration" );

  add( &TimeFrequencyZap::get_polarizations,
       &TimeFrequencyZap::set_polarizations,
       "pols", "Polarizations to analyze" );
  
  add( &TimeFrequencyZap::get_report,
       &TimeFrequencyZap::set_report,
       "report", "Print one-line report to stdout" );

  add( &TimeFrequencyZap::get_filename,
       &TimeFrequencyZap::set_filename,
       "fname", "Name of file to which stats are printed" );

  add( &TimeFrequencyZap::get_aux_filename,
       &TimeFrequencyZap::set_aux_filename,
       "aname", "Name of file to which auxiliary data are printed" );
}

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::TimeFrequencyZap::TimeFrequencyZap ()
{
  expression = "off:rms";
  regions_from_total = true;
  pscrunch = false;
  logarithmic = false;
 
  fscrunch_factor.disable_scrunch();
  bscrunch_factor.disable_scrunch();
  
  polns = ""; // Defaults to all

  // TODO make this a config option
  smoother = new DoubleMedian;

  // TODO this too
  masker = new SumThreshold;

  max_iterations = 1;
  recompute = false;
  recompute_original = false;
  
  nmasked = 0;
  nmasked_original = 0;
  
  report = false;
}

void delete_edges (Pulsar::Archive* data, const ScrunchFactor& factor)
{
  DEBUG("delete_edges archive=" << (void*) data);

  unsigned nchan = data->get_nchan();
  unsigned delete_nchan = factor.get_nscrunch (nchan);
  
  vector<unsigned> channels (delete_nchan);
  unsigned half_factor = delete_nchan/2;
  for (unsigned i=0; i<half_factor; i++)
  {
    channels[i] = nchan -i -1;
    channels[i+half_factor] = half_factor -i -1;
  }

  if (Archive::verbose > 2)
  {
    cerr << "delete chans:";
    for (unsigned i=0; i<delete_nchan; i++)
      cerr << " " << channels[i];
    cerr << endl;
  }
 
  assert( channels[delete_nchan-1] == 0 );

  unsigned nsubint = data->get_nsubint();

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
  DEBUG("copy_weights to=" << (void*) to << " from=" << (void*) from);
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
  DEBUG("TimeFrequencyZap::transform archive=" << (void*) archive);

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
  
  if ((regions_from_total || fscrunch_factor.scrunch_enabled())
      && !data->get_dedispersed())
  {
    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform need to dedisperse" << endl;

    // Need to make sure we are using a dedispersed version of the Archive

    /* optimization: when transform is called multiple times on the same
       archive, it can improve performance to perform this step only once.
       Therefore, save the dedispersed archive and re-use it if possible. */

    if (last_dedispersed == data)
    {
      if (Archive::verbose > 2)
        cerr << "TimeFrequencyZap::transform re-use dedispersed clone" << endl;

      // data weights were likely changed on last call
      copy_weights (dedispersed_clone, data);
      data = dedispersed_clone;
      cloned = true;
    }
    else
    {
      if (Archive::verbose > 2)
        cerr << "TimeFrequencyZap::transform clone and dedisperse" << endl;

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
    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform jobs='" << jobs << "'" << endl;
    
    StandardOptions processor;
    processor.add_job (jobs);

    if (!cloned)
      data = archive->clone();
    cloned = true;
    
    processor.process ( data );
    data = processor.result();
  }

  unsigned total_masked = 0;
  unsigned iterations = 0;

  do {
    iteration (data);
    total_masked += nmasked_original;
    iterations ++;
  }
  while (recompute_original && nmasked_original && iterations < max_iterations);

  if (cloned)
    copy_weights (archive, data);
  
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

void Pulsar::TimeFrequencyZap::iteration (Archive* archive)
{
  DEBUG("TimeFrequencyZap::iteration archive=" << (void*) archive);

  Reference::To<Archive> data = archive;
  Reference::To<Archive> backup = data;

  if (statistic && bscrunch_factor.scrunch_enabled())
  {
    ArchiveComparisons* compare = dynamic_cast<ArchiveComparisons*> (statistic.get());
    if (compare)
    {
      if (Archive::verbose > 2)
	cerr << "TimeFrequencyZap::transform"
	  " bscrunch by " << bscrunch_factor << endl;
      
      compare->set_bscrunch (bscrunch_factor);
    }
  }
  
  if (fscrunch_factor.scrunch_enabled())
  {
    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform"
	" fscrunch by " << fscrunch_factor << " nbin=" << archive->get_nbin() << endl;

    /*
      If the ArchiveComparison is based on a generalized chi-squared statistic,
      or anything else that is based on a sample covariance matrix, then it is
      best to compute that covariance matrix before fscrunching in order to
      maximize the rank (condition) of the matrix.
     */
    ArchiveComparisons* compare = 0;

    if (statistic)
      compare = dynamic_cast<ArchiveComparisons*> (statistic.get());

    if (compare)
    {
      compare->set_setup_Archive (backup);
      recompute_original = recompute && compare->get_setup ();
    }
    
    data = backup->clone();
    Pulsar::fscrunch (data.get(), fscrunch_factor);

    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform"
      " after fscrunch, nchan=" << data->get_nchan() << endl;
  }

  if (Archive::verbose > 2)
    cerr << "TimeFrequencyZap::transform computing mask" << endl;

  compute_mask (data);

  nmasked_original = nmasked_during_iterations;
      
  if (fscrunch_factor.scrunch_enabled() && data->get_nchan() > 1)
  {
    // back up the weights mask
    std::vector<float> mask0 = mask;
    unsigned nchan0 = nchan;
    
    // compute another mask with the fscrunched channel boundaries offset by 0.5*chbw
    
    data = backup->clone();

    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform delete edges" << endl;
    
    delete_edges (data, fscrunch_factor);

    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform"
        " fscrunch by " << fscrunch_factor << endl;
    
    Pulsar::fscrunch (data.get(), fscrunch_factor);

    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform computing offset mask" << endl;

    compute_mask (data);

    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::transform applying offset mask" << endl;

    unsigned offset = fscrunch_factor.get_nscrunch (nchan) / 2;
    apply_mask (archive, fscrunch_factor, offset);

    /*
      nmasked_original is reset here because clone is masked during iterations
    */
    nmasked_original = nmasked;

    // restore the original mask
    mask = mask0;
    nchan = nchan0;
  }

  if (Archive::verbose > 2)
    cerr << "TimeFrequencyZap::transform applying mask" << endl;

  apply_mask (archive, fscrunch_factor);
  nmasked_original += nmasked;
}


void Pulsar::TimeFrequencyZap::compute_mask (Archive* data)
{
  DEBUG("TimeFrequencyZap::compute_mask archive=" << (void*) data);

  // Size arrays
  nchan = data->get_nchan();
  nsubint = data->get_nsubint();

  TextInterface::parse_indeces(pol_i, polns, data->get_npol());
  //npol = data->get_npol();
  npol = pol_i.size();

  if (Archive::verbose > 2)
    cerr << "TimeFrequencyZap::compute_mask nsubint=" << nsubint
         << " nchan=" << nchan << " npol=" << npol << endl;
  
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
  nmasked_during_iterations = 0;

  compute_subint.clear();
  compute_chan.clear();
  
  do 
  {
    if (Archive::verbose > 2)
      cerr << "TimeFrequencyZap::compute_mask iteration=" << iter << endl;

    if (iter == 0 || recompute)
    {
      if (iter > 0)
      {
	apply_mask (data);
	nmasked_during_iterations += nmasked;
      }
      
      compute_stat (data);
    }
    
    // sets nmasked
    update_mask();

    iter ++;
  }
  while (iter < max_iterations && nmasked > 0 && !recompute_original);

  if (Archive::verbose > 2)
    cerr << "TimeFrequencyZap::compute_mask nmasked=" << nmasked_during_iterations << endl;
}



void Pulsar::TimeFrequencyZap::compute_stat (Archive* data)
{
  DEBUG("TimeFrequencyZap::compute_stat archive=" << (void*) data);

  if (Archive::verbose > 2)
    cerr << "TimeFrequencyZap::compute_stat nsubint=" << nsubint
         << " nchan=" << nchan << endl;

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

    if (aux_filename != "")
    {
      cerr << "TimeFrequencyZap::compute_stat opening "
	"'" << aux_filename << "'" << endl;
      
      FILE* f = fopen (aux_filename.c_str(), "w");
      statistic->set_file (f);

      // disable writing on the next iteration
      aux_filename = "";
    }
    
    ArchiveComparisons* compare = 0;
    compare = dynamic_cast<ArchiveComparisons*> (statistic.get());
    
    if (compare)
    {
      compare->set_compute_subint (compute_subint);
      compare->set_compute_chan (compute_chan);
    }
  }

  FILE* fptr = 0;
  
  if (filename != "")
  {
    cerr << "TimeFrequencyZap::compute_stat opening "
      "'" << filename << "'" << endl;
    
    fptr = fopen (filename.c_str(), "w");

    // disable writing on the next iteration
    filename = "";

    // cerr << "TimeFrequencyZap::compute_stat file opened" << endl;
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
	  // cerr << "calling Statistic::get" << endl;
          fval = statistic->get();
	  // cerr << "Statistic::get returned" << endl;
          if (logarithmic)
            fval = log(fval);
        }
        else
        {
	  Reference::To<const Profile> prof;
	  prof = subint->get_Profile(pol_i[ipol],ichan);

          stats->set_Profile(prof);
          string val = process(parser,expression);
          fval = fromstring<float>(val);
        }

        if (Archive::verbose > 2)
	  cerr << "isub=" << isub << " ichan=" << ichan << " ipol=" << ipol
	       << " fval=" << fval << endl;

        stat[idx(isub,ichan,ipol)] = fval;

	if (fptr)
	  fprintf (fptr, "%u %u %u %g\n", isub, ichan, ipol, fval);
      }
    }

    if (fptr)
      fprintf (fptr, "\n");
  }

  if (fptr)
  {
    fclose (fptr);
    fptr = 0;
  }
  
  // cerr << "calling Statistic::fclose" << endl;
  if (statistic)
    statistic->fclose ();
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
					   const ScrunchFactor& factor,
					   unsigned chan_offset)
{
  DEBUG("TimeFrequencyZap::apply_mask archive=" << (void*) archive);

  if (Archive::verbose > 2)
    cerr << "TimeFrequencyZap::apply_mask"
         << " fscrunch=" << factor << " chan_offset=" << chan_offset
         << endl;
  
  // count weights changed to zero
  nmasked = 0;

  unsigned nscrunch = factor.get_nscrunch (archive->get_nchan());

  compute_subint.resize (nsubint);
  std::fill (compute_subint.begin(), compute_subint.end(), false);

  compute_chan.resize (nchan);
  std::fill (compute_chan.begin(), compute_chan.end(), false);
  
  // apply mask back to original archive
  for (unsigned isub=0; isub<nsubint; isub++) 
  {
    Integration* subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++) 
    {
      float wt = mask[idx(isub,ichan)];
      if (nscrunch && wt != 0.0)
      {
	// change the weights of the original archive only if setting to zero
	// so that the weights are not set to that of the fscrunched data
	DEBUG("TimeFrequencyZap::apply_mask isub=" << isub << " wt=" << wt << " skip");
	continue;
      }

      DEBUG("TimeFrequencyZap::apply_mask isub=" << isub << " ichan=" << ichan << " wt=" << wt << " nscr=" << nscrunch << " off=" << chan_offset);

      for (unsigned jchan=0; jchan < nscrunch; jchan++)
      {
	unsigned ch = ichan*nscrunch + jchan + chan_offset;
        if (ch >= subint->get_nchan())
          continue;

	if (wt == 0 && subint->get_weight(ch) != 0)
	{
	  nmasked ++;
	  compute_subint[isub] = true;
	  compute_chan[ichan] = true;
	}
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
