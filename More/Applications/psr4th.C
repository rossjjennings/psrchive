/***************************************************************************
 *
 *   Copyright (C) 2008 - 2014 by Willem van Straten
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
#include "Pulsar/CovarianceMatrix.h"
#include "Pulsar/PhaseResolvedHistogram.h"

#include "Matrix.h"
#include "Stokes.h"
#include <assert.h>

using namespace std;

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

  //! Output the results
  void finalize ();

protected:

  class result
  {
    bool cross_covariance;
    
  public:
    //! Array of 4x4 fourth moments - one for each pulse phase bin
    std::vector< Matrix<4,4,double> > stokes_squared;

    //! Array of M 4x4 cross covariances, where M = nbin * (nbin-1) / 2
    std::vector< Matrix<4,4,double> > stokes_crossed;

    std::vector< Stokes<double> > stokes;

    uint64_t count;
    float histogram_threshold;

    result () { cross_covariance = false; count = 0; }

    void set_cross_covariance (bool flag);
    
    //! Resize arrays and initialize to zero
    void resize (unsigned nbin);
    void set_histogram_pa (unsigned nbin);
    void set_histogram_el (unsigned nbin);

    void histogram_pa (const Pulsar::PolnProfile*);
    void histogram_el (const Pulsar::PolnProfile*);

    Matrix<4,4,double> get_covariance (unsigned ibin);
    Matrix<4,4,double> get_cross_covariance (unsigned ibin, unsigned jbin);
    
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
  bool cross_covariance;
  
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

  - UnloadOptions (-e -m etc.): an option set that provides standard
  options for unloading data.

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
  cross_covariance = false;
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

  arg = menu.add (cross_covariance, "c");
  arg->set_help ("compute the cross covariances between phase bins");

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

  archive->convert_state( Signal::Stokes );

  if (!output)
  {
    string output_format = "PSRFITS";
    output = Pulsar::Archive::new_Archive (output_format);  
    output->copy (*archive);
    output->resize(1);

    results.resize (nchan);
    for (unsigned ichan = 0; ichan < nchan; ichan++)
    {
      results[ichan].set_cross_covariance (cross_covariance);
      results[ichan].resize (nbin);

      if (histogram_pa)
	results[ichan].set_histogram_pa( histogram_pa );
      if (histogram_el)
	results[ichan].set_histogram_el( histogram_el );

      results[ichan].histogram_threshold = histogram_threshold;
    }
  }

  if (output->get_nchan() != nchan)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nchan = %u != required nchan = %u",
		 nchan, output->get_nchan());

  if (output->get_nbin() != nbin)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nbin = %u != required nbin = %u",
		 nbin, output->get_nbin());

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Reference::To<Pulsar::Integration> subint = archive->get_Integration (isub);
    integration_length += subint->get_duration ();

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
        continue;

      Reference::To<Pulsar::PolnProfile> profile 
	= subint->new_PolnProfile (ichan);

      for (unsigned ibin=0; ibin < nbin; ibin++)
      {
	Stokes<double> S = profile->get_Stokes (ibin);

	results[ichan].stokes[ibin] += S;
	results[ichan].stokes_squared[ibin] += outer(S,S);
      }

      if (cross_covariance)
      {
	unsigned icross = 0;
	for (unsigned ibin=0; ibin<nbin; ibin++)
	  for (unsigned jbin=ibin+1; jbin<nbin; jbin++)
	  {
	    Stokes<double> Si = profile->get_Stokes (ibin);
	    Stokes<double> Sj = profile->get_Stokes (jbin);

	    results[ichan].stokes_crossed[icross] += outer(Si,Sj);
	    icross ++;
	  }

	assert (icross == results[ichan].stokes_crossed.size());
      }
      
      results[ichan].count ++;

      if (histogram_pa)
	results[ichan].histogram_pa (profile);

      if (histogram_el)
	results[ichan].histogram_el (profile);
    }
  }
}

void psr4th::finalize()
{
  unsigned nbin = output->get_nbin();
  unsigned nchan = output->get_nchan();
  unsigned npol = 4;
  unsigned nmoment = 10;

  std::string filename = "psr4th.ar";
  
  Pulsar::Integration* subint = output->get_Integration (0);
  subint->set_duration( integration_length );

  if (cross_covariance && nchan == 1)
  {
    cerr << "psr4th: cross covariance" << endl;
    
    unsigned ichan = 0;
    
    Pulsar::CovarianceMatrix* matrix = new Pulsar::CovarianceMatrix;
    matrix->set_nbin(nbin);
    matrix->set_npol(npol);
    matrix->resize();

    vector<double>& data = matrix->get_data();
    unsigned idat=0;
    
    for (unsigned ibin = 0; ibin < nbin ; ibin ++)
    {
      for (unsigned jbin = ibin; jbin < nbin ; jbin ++)
      {
	Matrix<4,4,double> covar;
 	covar = results[ichan].get_cross_covariance (ibin, jbin);

	for (unsigned ipol=0; ipol < npol; ipol++)
	  for (unsigned jpol = (ibin == jbin) ? ipol : 0; jpol < npol ; jpol++)
	  {
	    data.at(idat) = covar[ipol][jpol];
	    idat ++;
	  }
      }
    }

    assert (idat == data.size());

    cerr << "add CovarianceMatrix extension" << endl;
    output->add_extension( matrix );

    filename = "psr4th_covar.ar";
  }
      
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    Pulsar::PolnProfile* profile = subint->new_PolnProfile (ichan);

    Reference::To<Pulsar::MoreProfiles> more = new Pulsar::FourthMoments;
    more->resize( nmoment, nbin );

    if (histogram_pa)
    {
      cerr << "psr4th: adding position angle histogram extension" << endl;
      subint->get_Profile(0,ichan)->add_extension(results[ichan].hist_pa);
    }
    else if (histogram_el)
    {
      cerr << "psr4th: adding elipticity histogram extension" << endl;
      subint->get_Profile(0,ichan)->add_extension(results[ichan].hist_el);
    }
    else if (!cross_covariance)
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

      unsigned index=0;
      for (unsigned i=0; i<4; i++)
      {
	profile->get_Profile(i)->get_amps()[ibin] = mean[i];
	for (unsigned j=i; j<4; j++)
	{
	  more->get_Profile(index)->get_amps()[ibin] = covar[i][j];
	  index ++;
	}
      }
    }
  }

  Pulsar::CovarianceMatrix* ext = output->get<Pulsar::CovarianceMatrix>();
  cerr << "unloading npol=" << ext->get_npol() << " nbin=" << ext->get_nbin() << endl;
  output->unload (filename);
}

void psr4th::result::set_cross_covariance (bool flag)
{
  cross_covariance = flag;
  if (!cross_covariance || stokes.size() == 0)
    return;

  unsigned nbin = stokes.size();
  unsigned ncross = nbin * (nbin-1) / 2;
  stokes_crossed.resize( ncross );

  for (unsigned icross=0; icross < ncross; icross++)
    stokes_crossed[icross] = 0.0;
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
  set_cross_covariance (cross_covariance);
  count = 0;
}

void psr4th::result::set_histogram_pa (unsigned nbin)
{
  hist_pa = new Pulsar::PhaseResolvedHistogram;
  hist_pa->set_range (-90, 90);
  hist_pa->resize (nbin, stokes.size());
}

void psr4th::result::set_histogram_el (unsigned nbin)
{
  hist_el = new Pulsar::PhaseResolvedHistogram;
  hist_el->set_range (-1,1);
  hist_el->resize (nbin, stokes.size());
}


Matrix<4,4,double> psr4th::result::get_covariance (unsigned ibin)
{
  Matrix<4,4,double> meansq = stokes_squared [ibin];
  meansq /= count;

  Stokes<double> mean = get_mean(ibin);

  return meansq - outer(mean,mean);
}

Matrix<4,4,double> psr4th::result::get_cross_covariance (unsigned ibin,
							 unsigned jbin)
{
  if (ibin == jbin)
    return get_covariance (ibin);

  if (jbin < ibin)
    std::swap (ibin, jbin);

  unsigned nbin = stokes.size();

  // icross = nbin-1 + nbin-2 + nbin-3, where the number of terms = ibin
  // then offset by jbin, which starts at ibin+1
  unsigned icross = ibin * nbin - (ibin * (ibin+1) / 2) + (jbin - ibin - 1);

  if (icross >= stokes_crossed.size())
    throw Error (InvalidRange, "psr4th::result::get_cross_covariance",
		 "nbin=%u ibin=%u jbin=%u -> icross=%u >= ncross=%u",
		 nbin, ibin, jbin, icross, stokes_crossed.size());
  
  Matrix<4,4,double> meansq = stokes_crossed [icross];
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

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  psr4th program;
  return program.main (argc, argv);
}

