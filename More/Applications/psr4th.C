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

#include "Matrix.h"
#include "Stokes.h"

#include <assert.h>
#include <fstream>

using namespace std;
using namespace Pulsar;

//
//! Computes the phase-resolved 4x4 covariance matrix of the Stokes parameters
//
class psr4th : public Pulsar::Application
{
public:

  //! Default constructor
  psr4th ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! output the eigenvector profiles
  void output_eigenvectors (const Pulsar::Archive*);

  //! Output the results
  void finalize ();

protected:

  class result
  {
    unsigned cross_covariance_lags;

  public:

    void process (Pulsar::PolnProfile*);
    
    //! Array of 4x4 fourth moments - one for each pulse phase bin
    std::vector< Matrix<4,4,double> > stokes_squared;

    //! Array of nlag * M * 4x4 cross covariances, where M = nbin*(nbin+1)/2
    std::vector< Matrix<4,4,double> > stokes_crossed;

    std::vector< Stokes<double> > stokes;

    uint64_t count;
    float histogram_threshold;

    result () { cross_covariance_lags = 0; count = 0; }

    void set_cross_covariance_lags (unsigned);
    
    //! Resize arrays and initialize to zero
    void resize (unsigned nbin);
    void set_histogram_pa (unsigned nbin);
    void set_histogram_el (unsigned nbin);

    void histogram_pa (const Pulsar::PolnProfile*);
    void histogram_el (const Pulsar::PolnProfile*);

    Matrix<4,4,double> get_covariance (unsigned ibin);
    Matrix<4,4,double> get_cross_covariance (unsigned ibin, unsigned jbin,
					     unsigned ilag = 0);
    
    Stokes<double> get_mean (unsigned ibin);

    Reference::To<Pulsar::PhaseResolvedHistogram> hist_pa;
    Reference::To<Pulsar::PhaseResolvedHistogram> hist_el;
  };

  //! Array of results - one for each frequency channel
  std::vector<result> results;

  Reference::To<Pulsar::Archive> output;

  double integration_length;
  unsigned histogram_pa;
  unsigned histogram_el;
  float histogram_threshold;
  unsigned cross_covariance_lags;

  bool total_baseline;
  bool each_baseline;
  bool report_baseline;

  bool extract_eigenvectors;
  bool smooth_eigenvectors;
 
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
  add( new Pulsar::StandardOptions );
  integration_length = 0;
  histogram_pa = 0;
  histogram_el = 0;
  histogram_threshold = 3.0;
  cross_covariance_lags = 0;

  total_baseline = each_baseline = report_baseline = false;
  extract_eigenvectors = false;
  smooth_eigenvectors = false;
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

  arg = menu.add (total_baseline, "B");
  arg->set_help ("remove the baseline, found from integrated total");

  arg = menu.add (each_baseline, "b");
  arg->set_help ("remove the baseline, found from each sub-integration");

  arg = menu.add (report_baseline, "r");
  arg->set_help ("report baselines in psr4th_baselines.txt");

  arg = menu.add (extract_eigenvectors, "e");
  arg->set_help ("extract eigenvectors from psr4th.fits");

  arg = menu.add (smooth_eigenvectors, "smooth");
  arg->set_help ("smooth eigenvectors by maximizing projections");

  // // add an option that enables the user to set the source name with -name
  // arg = menu.add (scale, "name", "string");
  // arg->set_help ("set the source name to 'string'");
}


/*!

  Scale every profile and optionally set the source name

*/

void psr4th::process (Pulsar::Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  unsigned nbin = archive->get_nbin();
  unsigned nchan = archive->get_nchan();

  if (extract_eigenvectors)
  {
    output_eigenvectors (archive);
    return;
  }

  archive->convert_state( Signal::Stokes );

  Reference::To<Archive> tscrunched = archive->tscrunched();

  if (!output)
  {
    string output_format = "PSRFITS";
    output = Pulsar::Archive::new_Archive (output_format);  
    output->copy (tscrunched);
    
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
  }
  else
  {
    output->append (tscrunched);
    output->tscrunch();
  }

  if (total_baseline)
    archive->remove_baseline();

  if (output->get_nchan() != nchan)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nchan = %u != required nchan = %u",
		 nchan, output->get_nchan());

  if (output->get_nbin() != nbin)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nbin = %u != required nbin = %u",
		 nbin, output->get_nbin());

  ofstream baselines;
  if (each_baseline && report_baseline)
    baselines.open( "psr4th_baselines.txt");

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Reference::To<Pulsar::Integration> subint = archive->get_Integration (isub);
    integration_length += subint->get_duration ();

    if (each_baseline)
    {
      if (report_baseline)
      {
	vector< std::vector< Estimate<double> > > mean;
	subint->baseline_stats( &mean );

	for (unsigned i=0; i<4; i++)
	  baselines << mean[i][0].val << " ";
	baselines << endl;
      }
      
      subint->remove_baseline();
    }

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
        continue;

      Reference::To<Pulsar::PolnProfile> profile 
	= subint->new_PolnProfile (ichan);

      results[ichan].process (profile);
    }
  }
}


void psr4th::result::process (Pulsar::PolnProfile* profile)
{
  unsigned nbin = profile->get_nbin();
  
  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    Stokes<double> S = profile->get_Stokes (ibin);

    stokes[ibin] += S;
    stokes_squared[ibin] += outer(S,S);
  }

  if (cross_covariance_lags)
  {
    unsigned icross = 0;
    for (unsigned ibin=0; ibin<nbin; ibin++)
      for (unsigned jbin=ibin+1; jbin<nbin; jbin++)
      {
	Stokes<double> Si = profile->get_Stokes (ibin);
	Stokes<double> Sj = profile->get_Stokes (jbin);
	
	stokes_crossed[icross] += outer(Si,Sj);
	icross ++;
      }
    
    assert (icross == stokes_crossed.size());
  }
  
  count ++;
  
  if (hist_pa)
    histogram_pa (profile);
  
  if (hist_el)
    histogram_el (profile);
}


void dump (Pulsar::MoreProfiles* hist)
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

void psr4th::finalize()
{
  if (extract_eigenvectors)
    return;

  unsigned nbin = output->get_nbin();
  unsigned nchan = output->get_nchan();
  unsigned npol = 4;
  unsigned nmoment = 10;

  std::string filename = "psr4th.ar";
  
  Pulsar::Integration* subint = output->get_Integration (0);
  subint->set_duration( integration_length );

  if (cross_covariance_lags && nchan == 1)
  {
    cerr << "psr4th: cross covariance" << endl;

    unsigned nlag = cross_covariance_lags;
    unsigned ichan = 0;
    
    Pulsar::CrossCovarianceMatrix* matrix = new Pulsar::CrossCovarianceMatrix;
    matrix->set_nbin(nbin);
    matrix->set_npol(npol);
    matrix->set_nlag(nlag);
    
    matrix->resize();

    vector<double>& data = matrix->get_data();
    unsigned idat=0;
    
    for (unsigned ilag = 0; ilag < nlag; ilag ++)
    {
      for (unsigned ibin = 0; ibin < nbin ; ibin ++)
      {
	for (unsigned jbin = ibin; jbin < nbin ; jbin ++)
	{
	  Matrix<4,4,double> covar;
	  covar = results[ichan].get_cross_covariance (ibin, jbin, ilag);

	  for (unsigned ipol=0; ipol < npol; ipol++)
	  {
	    // on the diagonal, take only the upper triangle
	    unsigned startpol = (ibin == jbin) ? ipol : 0;
	    for (unsigned jpol = startpol; jpol < npol ; jpol++)
	    {
	      data.at(idat) = covar[ipol][jpol];
	      idat ++;
	    }
	  }
	}
      }
    }
    
    assert (idat == data.size());

    cerr << "add CrossCovarianceMatrix extension" << endl;
    output->add_extension( matrix );

    filename = "psr4th_covar.ar";
  }
      
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    // Pulsar::PolnProfile* profile = subint->new_PolnProfile (ichan);

    Reference::To<Pulsar::MoreProfiles> more = new Pulsar::FourthMoments;
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
    }

    for (unsigned ibin = 0; ibin < nbin ; ibin ++)
    {
      Matrix<4,4,double> covar;
      covar = results[ichan].get_covariance (ibin);

#if 0 
      Stokes<double> mean = results[ichan].get_mean (ibin);

      /*
	Normalize the covariance matrix so that it represents the
	covariances of the mean Stokes parameters (as opposed to the
	covariances of the single-pulse Stokes parameters).  This is
	done so that the variances of the off-pulse baselines of the
	mean Stokes parameters will be equal to the mean of the
	off-pulse baselines of the variances of the Stokes parameters.
      */
      covar /= results[ichan].count;
#endif

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
}

void psr4th::result::set_cross_covariance_lags (unsigned nlag)
{
  cross_covariance_lags = nlag;
  if (!cross_covariance_lags || stokes.size() == 0)
    return;

  unsigned nbin = stokes.size();
  unsigned ncross = nbin * (nbin+1) / 2;
  unsigned nelement = ncross * nlag;
  stokes_crossed.resize( nelement );

  for (unsigned ielement=0; ielement < nelement; ielement++)
    stokes_crossed[ielement] = 0.0;
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
  hist_pa = new Pulsar::PhaseResolvedHistogram;
  hist_pa->set_range (-90, 90);
  hist_pa->resize (nhist, stokes.size());

  // dump (hist_pa);
}

void psr4th::result::set_histogram_el (unsigned nhist)
{
  hist_el = new Pulsar::PhaseResolvedHistogram;
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

Matrix<4,4,double> psr4th::result::get_cross_covariance (unsigned ibin,
							 unsigned jbin,
							 unsigned ilag)
{
  if (jbin < ibin)
    std::swap (ibin, jbin);

  unsigned nbin = stokes.size();

  // icross = nbin + nbin-1 + nbin-2 + ..., where the number of terms = ibin
  // then offset by jbin, which starts at ibin
  unsigned icross = ibin * nbin - ((ibin * (ibin+1)) / 2) + (jbin - ibin);
  unsigned ncross = (nbin * (nbin+1)) / 2;
  unsigned offset = ilag * ncross + icross;
  
  if (offset >= stokes_crossed.size())
    throw Error (InvalidRange, "psr4th::result::get_cross_covariance",
		 "nbin=%u ibin=%u jbin=%u -> icross=%u >= ncross=%u",
		 nbin, ibin, jbin, icross, stokes_crossed.size());
  
  Matrix<4,4,double> meansq = stokes_crossed [offset];
  meansq /= count;

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

void psr4th::result::histogram_pa (const Pulsar::PolnProfile* profile)
{
  unsigned negative_count = 0;

  std::vector< Estimate<double> > PA;
  profile->get_orientation (PA, histogram_threshold);

  Reference::To<Pulsar::Profile> linear = new Pulsar::Profile;
  profile->get_linear (linear);

  float* L = linear->get_amps();

  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (PA[ibin].get_variance() == 0.0)
      continue;

    Pulsar::Profile* of = hist_pa->at( PA[ibin].get_value() );
    of->get_amps()[ibin] += L[ibin];

    if (L[ibin] < 0.0)
      negative_count ++;
  }
  //cerr << "psr4th::result::histogram_pa negatives=" << negative_count << endl;
}

void psr4th::result::histogram_el (const Pulsar::PolnProfile* profile)
{
  std::vector< Estimate<double> > epsilon;
  profile->get_ellipticity (epsilon, histogram_threshold);

  Reference::To<Pulsar::Profile> polarized = new Pulsar::Profile;
  profile->get_polarized (polarized);

  float* P = polarized->get_amps();

  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (epsilon[ibin].get_variance() == 0.0)
      continue;

    Pulsar::Profile* of = hist_el->at( sin(epsilon[ibin].get_value()*M_PI/90) );
    of->get_amps()[ibin] += P[ibin];
  }
}

void psr4th::output_eigenvectors (const Pulsar::Archive* data)
{
  vector< Reference::To<Pulsar::Archive> > eigen (3);
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

