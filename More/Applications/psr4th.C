/***************************************************************************
 *
 *   Copyright (C) 2008 - 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/FourthMoments.h"
#include "Pulsar/CrossCovarianceMatrix.h"
#include "Pulsar/PhaseResolvedHistogram.h"
#include "Pulsar/FourthMomentStats.h"
#include "Pulsar/StokesCrossCovariance.h"

#include "BinaryStatistic.h"
#include "Matrix.h"
#include "Stokes.h"
#include "RealTimer.h"

#include <assert.h>
#include <fstream>

using namespace std;
using namespace Pulsar;

//
//! Computes the phase-resolved 4x4 covariance matrix of the Stokes parameters
//
class psr4th : public Application
{
public:

  //! Default constructor
  psr4th ();

  //! Load method does additional pre-processing
  Archive* load (const std::string& filename);

  //! Stage 1: Accumulate the mean
  void process (Archive*);

  //! Stage 2: Accumulate any running means (called during finalize)
  void compute_means (Archive*);

  //! Stage 3: Compute moments
  void compute_moments (Archive*);

  //! output the eigenvector profiles
  void output_eigenvectors (const Archive*);

  //! Output the results
  void finalize ();

  //! Compute the running mean of the baseline estimates
  void compute_running_mean_baseline ();

protected:

  class result
  {
    unsigned cross_covariance_lags;

  public:

    void process (PolnProfile*, double weight = 1.0);
    
    //! Array of 4x4 fourth moments - one for each pulse phase bin
    std::vector< Matrix<4,4,double> > stokes_squared;

    std::vector< Stokes<double> > stokes;

    //! Last nlag profiles
    std::vector< Reference::To<PolnProfile> > profiles;
    
    uint64_t count;
    float histogram_threshold;

    result () { cross_covariance_lags = 0; count = 0; }

    void set_cross_covariance_lags (unsigned);
    
    //! Resize arrays and initialize to zero
    void resize (unsigned nbin);
    void set_histogram_pa (unsigned nbin);
    void set_histogram_el (unsigned nbin);

    void histogram_pa (const PolnProfile*);
    void histogram_el (const PolnProfile*);

    void compute_cross_covariance ();
    
    Matrix<4,4,double> get_covariance (unsigned ibin);
    Matrix<4,4,double> get_cross_covariance (unsigned ibin, unsigned jbin,
					     unsigned ilag = 0);
    
    Stokes<double> get_mean (unsigned ibin);

    Reference::To<PhaseResolvedHistogram> hist_pa;
    Reference::To<PhaseResolvedHistogram> hist_el;

    //! Manages array of nlag * nbin * nbin * 4 * 4 cross covariances
    StokesCrossCovariance cross_covar;
  };

  //! Array of results - one for each frequency channel
  std::vector<result> results;

  //! Input data filenames
  vector<string> input_filenames;

  //! The output archive
  Reference::To<Archive> output;

  //! The currently open input archive
  Reference::To<Archive> current;
  unsigned isub_offset;

  double integration_length;
  double total_nsample;
  unsigned total_nsubint;
  
  unsigned remove_running_mean_profile;
  Reference::To<Archive> current_extract;
  Reference::To<Archive> running_mean_profiles;

  unsigned histogram_pa;
  unsigned histogram_el;
  float histogram_threshold;
  unsigned cross_covariance_lags;

  bool total_baseline;
  bool each_baseline;

  unsigned remove_running_mean_baseline;
  // estimated baselines[isubint][ipol][ichan]
  vector< vector< vector< Estimate<double> > > > baselines;
  // smoothed mean baselines
  vector< vector< vector< MeanEstimate<double> > > > mean_baselines;

  bool report_baseline;
  std::ofstream baselines_out;

  bool accumulation_required () const { return total_baseline || remove_running_mean_baseline || remove_running_mean_profile; }

  bool extract_eigenvectors;
  bool smooth_eigenvectors;
  bool report_execution_times;

  bool compute_weighted_mean_covariance;
 
  //! Add command line options
  void add_options (CommandLine::Menu&);
};


/*!

  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "psr4th -h"

  This constructor makes use of

  - StandardOptions (-j -J etc.): an option set that provides standard
  preprocessing with the pulsar command language interpreter.

  This constructor also sets the default values of the attributes that
  are unique to the program.

*/

psr4th::psr4th ()
  : Application ("psr4th", "psr4th psrchive program")
{
  add( new StandardOptions );

  integration_length = 0;
  total_nsample = 0;
  total_nsubint = 0;
  isub_offset = 0;

  histogram_pa = 0;
  histogram_el = 0;
  histogram_threshold = 3.0;
  cross_covariance_lags = 0;
  remove_running_mean_profile = 0;

  total_baseline = each_baseline = report_baseline = false;
  remove_running_mean_baseline = 0;

  extract_eigenvectors = false;
  smooth_eigenvectors = false;
  report_execution_times = false;
  compute_weighted_mean_covariance = false;
}


/*!

  Add application-specific command-line options.

*/

void psr4th::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  // add an option that enables the user to set the scale with -s
  arg = menu.add (histogram_pa, "pa", "nbin");
  arg->set_help ("compute the position angle histogram, divided in nbin");

  arg = menu.add (histogram_el, "el", "nbin");
  arg->set_help ("compute the elipticity histogram, divided in nbin");

  arg = menu.add (histogram_threshold, "t", "sigma");
  arg->set_help ("threshold applied when computing histograms");

  arg = menu.add (cross_covariance_lags, "c", "nlag");
  arg->set_help ("compute the cross covariances between phase bins");

  arg = menu.add (compute_weighted_mean_covariance, "w");
  arg->set_help ("compute the weighted covariance");

  arg = menu.add (total_baseline, "B");
  arg->set_help ("remove the baseline, found from integrated total");

  arg = menu.add (each_baseline, "b");
  arg->set_help ("remove the baseline, found from each sub-integration");

  arg = menu.add (remove_running_mean_baseline, "d", "nsub");
  arg->set_help ("remove the running mean baseline of nsub sub-integrations");

  arg = menu.add (remove_running_mean_profile, "m", "nsub");
  arg->set_help ("remove the running mean profile of nsub sub-integrations");
  
  arg = menu.add (report_baseline, "r");
  arg->set_help ("report baselines in psr4th_baselines.txt");

  arg = menu.add (extract_eigenvectors, "e");
  arg->set_help ("extract eigenvectors from psr4th.fits");

  arg = menu.add (smooth_eigenvectors, "smooth");
  arg->set_help ("smooth eigenvectors by maximizing projections");

  arg = menu.add (report_execution_times, "runtime");
  arg->set_help ("report execution times");

  // // add an option that enables the user to set the source name with -name
  // arg = menu.add (scale, "name", "string");
  // arg->set_help ("set the source name to 'string'");
}


Archive* psr4th::load (const string& filename)
{
  if (current && current->get_filename() == filename)
    return current;

  current = Application::load (filename);
  current->convert_state( Signal::Stokes );
  return current;
}

void psr4th::process (Archive* archive)
{
  if (extract_eigenvectors)
  {
    output_eigenvectors (archive);
    return;
  }

  // for now, simply add the data to the total
  Reference::To<Archive> tscrunched = archive->tscrunched();

  if (!output)
  {
    string output_format = "PSRFITS";
    output = Archive::new_Archive (output_format);  
    output->copy (tscrunched);

    unsigned nbin = archive->get_nbin();
    unsigned nchan = archive->get_nchan();
 
    results.resize (nchan);
    for (unsigned ichan = 0; ichan < nchan; ichan++)
    {
      results[ichan].set_cross_covariance_lags (cross_covariance_lags);
      results[ichan].resize (nbin);

      if (histogram_pa)
	results[ichan].set_histogram_pa( histogram_pa );
      if (histogram_el)
	results[ichan].set_histogram_el( histogram_el );

      results[ichan].histogram_threshold = histogram_threshold;
    }

    if (report_baseline)
      baselines_out.open( "psr4th_baselines.txt");
  }
  else
  {
    output->append (tscrunched);
    output->tscrunch();
  }

  if (!total_baseline && accumulation_required())
    compute_means (archive);

  // save the filename for later processing during finalize
  input_filenames.push_back( archive->get_filename() );
}

// compute running means
void psr4th::compute_means (Archive* archive)
{
  Reference::To<Pulsar::PhaseWeight> baseline;

  if (total_baseline)
    baseline = output->baseline();

  unsigned nchan = archive->get_nchan();

  if (output->get_nchan() != nchan)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nchan = %u != required nchan = %u",
		 nchan, output->get_nchan());

  unsigned nbin = archive->get_nbin();

  if (output->get_nbin() != nbin)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nbin = %u != required nbin = %u",
		 nbin, output->get_nbin());


  unsigned nsub = archive->get_nsubint();
  unsigned npol = archive->get_npol();

#if 0
  ofstream baselines;
  if (report_baseline)
    baselines.open( "psr4th_baselines.txt");

  for (unsigned isub=0; isub < nsub; isub++)
  {
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      for (unsigned i=0; i<4; i++)
        baselines << baselines[isub][i][ichan].val << " ";
      baselines << endl;
    }
  }

#endif

  unsigned total_nsub = baselines.size();
  baselines.resize( total_nsub + nsub );

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Reference::To<Integration> subint = archive->get_Integration (isub);
    subint->baseline_stats( &(baselines[total_nsub+isub]), 0, baseline.ptr() );
  }

  if (remove_running_mean_profile > 1)
  {
    unsigned nsub_extract = remove_running_mean_profile;

    vector<unsigned> subints (nsub_extract);
    unsigned isub_start = 0;

    unsigned nsub_left_over = 0;
    if (current_extract)
    {
      /* if there are any extracted sub-integrations left-over 
         from the previous file, then top these up */
      nsub_left_over = current_extract -> get_nsubint ();
      if (verbose)
        cerr << "resuming with " << nsub_left_over << " left-over sub-integrations" << endl;
    }

    while (isub_start < nsub)
    {
      subints.resize (nsub_extract);
      unsigned isub = 0;
      for (isub=0; isub_start+isub < nsub && nsub_left_over+isub < nsub_extract; isub++)
        subints[isub] = isub_start + isub;

      if (verbose)
        cerr << "extract isub start=" << subints[0] << " end=" << subints[isub-1] << endl;

      subints.resize (isub);
      isub_start += isub;

      // left-overs are used only once
      nsub_left_over = 0;

      Reference::To<Archive> extract = archive->extract (subints);

      if (current_extract)
      {
        if (verbose)
          cerr << "appending " << isub << " subints to left-over extract with "
             << current_extract -> get_nsubint() << " subints" << endl;

        current_extract -> append (extract);
        if (current_extract->get_nsubint() == nsub_extract)
        {
          extract = current_extract;
          current_extract = 0;
        }
      }

      if (extract->get_nsubint() == nsub_extract)
      {
        if (verbose)
          cerr << "tscrunching complete extraction with " 
               << extract->get_nsubint() << " subints" << endl;

        extract->tscrunch ();

        if (running_mean_profiles)
          running_mean_profiles->append (extract);
        else
          running_mean_profiles = extract;
      }
      else
      {
        if (verbose)
          cerr << "saving left-over extraction with "
               << extract->get_nsubint() << " subints" << endl;

        // save any left-over sub-integrations for the next file
        current_extract = extract;
      }
    }
  }
}

void psr4th::compute_moments (Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  unsigned nbin = archive->get_nbin();

  Reference::To<Pulsar::PhaseWeight> baseline;

  if (total_baseline)
    baseline = output->baseline();

  if (verbose && running_mean_profiles)
    cerr << "removing running mean profiles nsub=" << running_mean_profiles->get_nsubint()
         << " isub_offset=" << isub_offset << endl;

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Reference::To<Integration> subint = archive->get_Integration (isub);

    // duration in seconds
    double duration = subint->get_duration();
    // bandwidth in Hertz
    double bandwidth = abs(subint->get_bandwidth() * 1e6 / subint->get_nchan());

    if (bandwidth == 0)
      throw Error (InvalidState, "psr4th::process",
		   "bandwidth unknown");

    if (duration <= 0)
      throw Error (InvalidState, "psr4th::process",
		   "integration length unknown");
    
    integration_length += duration;
    total_nsample += duration * bandwidth / nbin;
    total_nsubint ++;
    
    if (running_mean_profiles)
    {
      unsigned imean = (isub_offset + isub) / remove_running_mean_profile;
      if (imean >= running_mean_profiles->get_nsubint())
	imean = running_mean_profiles->get_nsubint() - 1;

      Reference::To<Integration> msub = running_mean_profiles->get_Integration (imean);

      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
	if (subint->get_weight(ichan) == 0)
	  continue;

	for (unsigned ipol=0; ipol < npol; ipol++)
	{
	  Pulsar::Profile* profile = subint->get_Profile (ipol, ichan);
	  Pulsar::Profile* mprof = msub->get_Profile (ipol, ichan);

	  profile->diff(mprof);
	}
      }
    }
    else if (remove_running_mean_baseline)
    {
      unsigned imean = (isub_offset + isub) / remove_running_mean_baseline;
      assert (imean < mean_baselines.size());

      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
        if (subint->get_weight(ichan) == 0)
          continue;

        // cerr << isub << "->" << imean << " ichan=" << ichan;
        for (unsigned ipol=0; ipol < npol; ipol++)
        {
          Pulsar::Profile* profile = subint->get_Profile (ipol, ichan);
          double offset = mean_baselines[imean][ipol][ichan].get_Estimate().get_value();
          // cerr << " " << offset; 
          profile->offset(-offset);
        }
        // cerr << endl;
      }
    }
    else if (total_baseline || each_baseline)
    {
      if (report_baseline && !accumulation_required())
      {
        vector< std::vector< Estimate<double> > > mean;
        subint->baseline_stats( &mean, 0, baseline.ptr() );
 
        for (unsigned i=0; i<4; i++)
          baselines_out << mean[i][0].val << " ";
        baselines_out << endl;
      }
    
      subint->remove_baseline( baseline.ptr() );
    }

    RealTimer clock;

    clock.start();
 
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
        continue;

      Reference::To<PolnProfile> profile 
	= subint->new_PolnProfile (ichan);

      double weight = 1.0;

      if (compute_weighted_mean_covariance)
        weight = subint->get_weight (ichan);

      results[ichan].process (profile, weight);
    }

    clock.stop();

    if (report_execution_times)
      cerr << "subint=" << isub << " time=" << clock << endl;
  }

  isub_offset += archive->get_nsubint();
}

void psr4th::result::process (PolnProfile* profile, double weight)
{
  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    Stokes<double> S = profile->get_Stokes (ibin);

    stokes[ibin] += weight * S;

    auto product = outer(S,S);
    product *= weight;

    stokes_squared[ibin] += product; // outer(S,S) * weight;
  }

  if (cross_covariance_lags)
  {
    profiles.insert (profiles.begin(), profile);
    if (profiles.size() > cross_covariance_lags)
      profiles.resize (cross_covariance_lags);
    
    for (unsigned ilag=0; ilag<profiles.size(); ilag++)
    {
      for (unsigned ibin=0; ibin<nbin; ibin++)
      {
	unsigned startbin = (ilag == 0) ? ibin : 0;
	
	for (unsigned jbin=startbin; jbin<nbin; jbin++)
	{
	  Matrix<4,4,double>& sum
	    = cross_covar.get_cross_covariance (ibin, jbin, ilag);

	  Stokes<double> Si = profiles[ilag]->get_Stokes (ibin);
	  Stokes<double> Sj = profile->get_Stokes (jbin);

          auto product = outer(Si,Sj);
          product *= weight;

	  sum += product; // outer(Si,Sj) * weight;
	}
      }
    }
  }
  
  count += weight;
  
  if (hist_pa)
    histogram_pa (profile);
  
  if (hist_el)
    histogram_el (profile);
}


void dump (MoreProfiles* hist)
{
  unsigned nprof = hist->get_size();
  unsigned nbin = hist->get_nbin();
  for (unsigned iprof=0; iprof < nprof; iprof++)
  {
    cerr << iprof;
    float* prof = hist->get_Profile(iprof)->get_amps();
    for (unsigned ibin=0; ibin<nbin; ibin++)
      cerr << " " << prof[ibin];
    cerr << endl;
  }
}

void psr4th::compute_running_mean_baseline ()
{
  if (verbose)
    cerr << "psr4th::compute_running_mean_baseline ndat=" << baselines.size() << endl;

  if (baselines.size() == 0)
    return;

  unsigned nscrunch = remove_running_mean_baseline;

  unsigned ndat = baselines.size();
  unsigned nmean = ndat / nscrunch;
  if (ndat % nscrunch)
    nmean ++;

  unsigned npol = baselines[0].size();
  assert (npol == 4);
  unsigned nchan = baselines[0][0].size();

  if (verbose)
    cerr << "psr4th::compute_running_mean_baseline nscrunch=" << nscrunch 
         << " nmean=" << nmean << " nchan=" << nchan << endl;

  mean_baselines.resize (nmean);

  unsigned idat = 0;
  for (unsigned imean = 0; imean < nmean; imean++)
  {
    mean_baselines[imean].resize (npol);
    for (unsigned ipol = 0; ipol < npol; ipol++)
    {
      mean_baselines[imean][ipol].resize (nchan);
      for (unsigned ichan = 0; ichan < nchan; ichan++)
        mean_baselines[imean][ipol][ichan] = baselines[idat][ipol][ichan];
    }

    unsigned iscrunch = 1;
    while (iscrunch < nscrunch && idat+iscrunch < ndat)
    {
      for (unsigned ipol = 0; ipol < npol; ipol++)
        for (unsigned ichan = 0; ichan < nchan; ichan++)
          mean_baselines[imean][ipol][ichan] += baselines[idat+iscrunch][ipol][ichan];

      iscrunch++;
    }
    idat += iscrunch;
  }
}

void psr4th::finalize()
{
  if (extract_eigenvectors)
    return;

  if (total_baseline && accumulation_required())
  {
    if (verbose)
      cerr << "psr4th::finalize computing baselines" << endl;

    for (unsigned ifile=0; ifile < input_filenames.size(); ifile++)
      compute_means ( load(filenames[ifile]) );
  }

  if (remove_running_mean_baseline)
  {
    if (verbose)
      cerr << "psr4th::finalize computing running mean baseline" << endl;
    compute_running_mean_baseline ();
  }

  if (verbose)
    cerr << "psr4th::finalize computing moments" << endl;

  for (unsigned ifile=0; ifile < input_filenames.size(); ifile++)
    compute_moments ( load(filenames[ifile]) );

  unsigned nbin = output->get_nbin();
  unsigned nchan = output->get_nchan();
  unsigned nmoment = 10;

  RealTimer clock;
  clock.start();

  std::string filename = "psr4th.ar";
  
  Integration* subint = output->get_Integration (0);
  subint->set_duration( integration_length );

  double nsample = total_nsample / total_nsubint;
  
  if (cross_covariance_lags && nchan == 1)
  {
    cerr << "psr4th: cross covariance" << endl;

    unsigned ichan = 0;
    results[ichan].compute_cross_covariance();
    
    CrossCovarianceMatrix* matrix = new CrossCovarianceMatrix;
    results[ichan].cross_covar.unload (matrix);
    
    cerr << "add CrossCovarianceMatrix extension" << endl;
    output->add_extension( matrix );

    filename = "psr4th_covar.ar";
  }
      
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    // PolnProfile* profile = subint->new_PolnProfile (ichan);

    Reference::To<FourthMoments> more = new FourthMoments;
    more->resize( nmoment, nbin );

    if (histogram_pa)
    {
      cerr << "psr4th: adding position angle histogram extension" << endl;
      subint->get_Profile(0,ichan)->add_extension(results[ichan].hist_pa);
      // dump (results[ichan].hist_pa);
    }
    else if (histogram_el)
    {
      cerr << "psr4th: adding elipticity histogram extension" << endl;
      subint->get_Profile(0,ichan)->add_extension(results[ichan].hist_el);
    }
    else if (!cross_covariance_lags)
      subint->get_Profile(0,ichan)->add_extension(more);

    if (results[ichan].count == 0)
    {
      subint->set_weight (ichan, 0.0);
      subint->get_Profile(0,ichan)->zero();
      more->set_nsample (0);
    }
    else
    {
      // cerr << "psr4th: nsample=" << nsample << endl;
      more->set_nsample (nsample);
    }
    
    for (unsigned ibin = 0; ibin < nbin ; ibin ++)
    {
      Matrix<4,4,double> covar;
      covar = results[ichan].get_covariance (ibin);

      unsigned index=0;
      for (unsigned i=0; i<4; i++)
      {
	// profile->get_Profile(i)->get_amps()[ibin] = mean[i];
	for (unsigned j=i; j<4; j++)
	{
	  more->get_Profile(index)->get_amps()[ibin] = covar[i][j];
	  index ++;
	}
      }
    }
  }

  output->unload (filename);

  clock.stop();
  if (report_execution_times)
    cerr << "output unloaded in " << clock << endl;

}

void psr4th::result::set_cross_covariance_lags (unsigned nlag)
{
  cross_covariance_lags = nlag;
  if (!cross_covariance_lags || stokes.size() == 0)
    return;
  
  cross_covar.set_nbin (stokes.size());
  cross_covar.set_nlag (cross_covariance_lags);
  cross_covar.resize ();
  cross_covar.set_all (0.0);
}
    
void psr4th::result::resize (unsigned nbin)
{
  stokes.resize (nbin);
  stokes_squared.resize (nbin);

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    stokes[ibin] = 0.0;
    stokes_squared[ibin] = 0.0;
  }

  // also does the resize
  set_cross_covariance_lags (cross_covariance_lags);
  count = 0;
}

void psr4th::result::set_histogram_pa (unsigned nhist)
{
  hist_pa = new PhaseResolvedHistogram;
  hist_pa->set_range (-90, 90);
  hist_pa->resize (nhist, stokes.size());

  // dump (hist_pa);
}

void psr4th::result::set_histogram_el (unsigned nhist)
{
  hist_el = new PhaseResolvedHistogram;
  hist_el->set_range (-1,1);
  hist_el->resize (nhist, stokes.size());
}


Matrix<4,4,double> psr4th::result::get_covariance (unsigned ibin)
{
  Matrix<4,4,double> meansq = stokes_squared [ibin];
  meansq /= count;

  Stokes<double> mean = get_mean(ibin);

  return meansq - outer(mean,mean);
}

void psr4th::result::compute_cross_covariance ()
{
  unsigned nbin = cross_covar.get_nbin();
  unsigned nlag = cross_covar.get_nlag();

#if _DEBUG
    cerr << "psr4th::result::compute_cross_covariance"
	 << " nbin=" << nbin << " nlag=" << nlag << endl;
#endif
    
  cross_covar.resize();
  
  for (unsigned ilag=0; ilag < nlag; ilag++)
  {
    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      unsigned startbin = (ilag == 0) ? ibin : 0;
      for (unsigned jbin=startbin; jbin < nbin; jbin++)
      {
	Matrix<4,4,double> set = get_cross_covariance (ibin, jbin, ilag);

	bool lock = true;
	cross_covar.set_cross_covariance (ibin, jbin, ilag, set, lock);

	Matrix<4,4,double> get;
	get = cross_covar.get_cross_covariance (ibin, jbin, ilag);

	if (set != get)
	{
	  if (ilag > 0 && ibin == 0 && jbin == 0)
	    cerr << "psr4th::result::compute_cross_covariance ilag=" << ilag
		 << " ibin=" << ibin << " jbin=" << jbin << " SET != GET\n"
		 << set << endl << get << endl;

	  throw Error (InvalidState,
		       "psr4th::result::compute_cross_covariance",
		       "set != get on ilag=%u ibin=%u jbin=%u",
		       ilag, ibin, jbin);
	}
      } // for each column
    } // for each row
  } // for each lag
}

Matrix<4,4,double> psr4th::result::get_cross_covariance (unsigned ibin,
							 unsigned jbin,
							 unsigned ilag)
{
  if (count <= ilag)
    throw Error (InvalidState, "psr4th::result::get_cross_covariance",
		 "count=%u >= ilag=%u", count, ilag);
  
  Matrix<4,4,double> meansq;
  meansq = cross_covar.get_cross_covariance (ibin, jbin, ilag);

  double lag_count = count - ilag;
  meansq /= lag_count;

  Stokes<double> imean = get_mean(ibin);
  Stokes<double> jmean = get_mean(jbin);

  return meansq - outer(imean,jmean);
}

Stokes<double> psr4th::result::get_mean (unsigned ibin)
{
  Stokes<double> mean = stokes [ibin];
  mean /= count;
  return mean;
}

void psr4th::result::histogram_pa (const PolnProfile* profile)
{
  unsigned negative_count = 0;

  std::vector< Estimate<double> > PA;
  profile->get_orientation (PA, histogram_threshold);

  Reference::To<Profile> linear = new Profile;
  profile->get_linear (linear);

  float* L = linear->get_amps();

  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (PA[ibin].get_variance() == 0.0)
      continue;

    Profile* of = hist_pa->at( PA[ibin].get_value() );
    of->get_amps()[ibin] += L[ibin];

    if (L[ibin] < 0.0)
      negative_count ++;
  }
  //cerr << "psr4th::result::histogram_pa negatives=" << negative_count << endl;
}

void psr4th::result::histogram_el (const PolnProfile* profile)
{
  std::vector< Estimate<double> > epsilon;
  profile->get_ellipticity (epsilon, histogram_threshold);

  Reference::To<Profile> polarized = new Profile;
  profile->get_polarized (polarized);

  float* P = polarized->get_amps();

  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (epsilon[ibin].get_variance() == 0.0)
      continue;

    Profile* of = hist_el->at( sin(epsilon[ibin].get_value()*M_PI/90) );
    of->get_amps()[ibin] += P[ibin];
  }
}

void psr4th::output_eigenvectors (const Archive* data)
{
  vector< Reference::To<Archive> > eigen (3);
  for (unsigned i=0; i<3; i++)
    eigen[i] = data->clone();

  for (unsigned isubint=0; isubint < data->get_nsubint(); isubint++)
  {
    Reference::To<const Integration> subint = data->get_Integration (isubint);

    vector< Reference::To<Integration> > subeigen (3);
    for (unsigned i=0; i<3; i++)
      subeigen[i] = eigen[i]->get_Integration (isubint);

    for (unsigned ichan=0; ichan < data->get_nchan(); ichan++)
    {
      Reference::To<const PolnProfile> profile;
      profile = subint->new_PolnProfile (ichan);
      Reference::To<FourthMomentStats> stats = new FourthMomentStats(profile);

      vector< Reference::To<PolnProfile> > peigen (3);
      for (unsigned i=0; i<3; i++)
        peigen[i] = subeigen[i]->new_PolnProfile (ichan);

      stats->eigen (peigen[0], peigen[1], peigen[2]);
      if (smooth_eigenvectors)
        stats->smooth_eigenvectors (peigen[0], peigen[1], peigen[2]);
    }
  }

  for (unsigned i=0; i<3; i++)
  {
    string filename = "psr4th_e" + tostring(i+1) + ".ar";
    eigen[i]->unload (filename);
  }
}

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  psr4th program;
  return program.main (argc, argv);
}

