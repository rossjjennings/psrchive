/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "MEAL/Polar.h"

#include "Pulsar/StokesPlot.h"

#include "statutil.h"
#include "strutil.h"
#include "dirutil.h"

#include <cpgplot.h>

#include <unistd.h>

using namespace std;
using namespace Pulsar;

void usage ()
{
  cerr << 
    "psrdiff - measure the difference between two pulse profiles \n"
    "\n"
    "psrdiff [options] filename[s]\n"
    "options:\n"
    " -d               StokesPlot difference\n"
    " -c min_chisq     StokesPlot difference only when reduced chisq > min \n"
    " -h               Help page \n"
#if HAVE_ARMADILLO
    " -L               print/compare the likelihood of data \n"
#endif
    " -M metafile      Specify list of archive filenames in metafile \n"
    " -X               print the reduced chi squared of the difference \n"
    " -q               Quiet mode \n"
    " -v               Verbose mode \n"
    " -V               Very verbose mode \n"
    "\n"
       << endl;
}

static bool verbose = false;
static bool fscrunch = false;
static bool tscrunch = false;
static bool pscrunch = false;

// print the reduced chi squared of the difference
bool print_reduced_chisq = false;

void diff_two (const std::string& fileA, const std::string& fileB);

int likelihood_analysis (vector<string>& filenames);

class Difference
{
protected:

  unsigned isubint = 0;
  unsigned ichan = 0;
  unsigned ipol = 0;

  Reference::To<const Archive> archive;
  Reference::To<const Profile> profile;

public:

  //! return the reduced chisq for an entire archive
  double reduced_chisq(const Archive*);

  //! Perform any setup for the current sub-integration
  virtual void set_subint (const Integration*) {}

  //! Return any additional weight information for the specified channel of the current sub-integration
  virtual unsigned get_weight (unsigned ichan) { return 1; }

  //! return the reduced chisq for the current isubint, ichan, and ipol
  /*! Defined by children. */
  virtual double chisq(const Profile*) = 0;
};

class TemplateDifference : public Difference
{
  Reference::To<const Integration> std_subint;
  vector<vector<double>> std_variance;
  vector<vector<double>> variance;

public:

  //! Set the template to which the data will be compared
  void set_template(const Integration* _template);

  //! Perform any setup for the current sub-integration
  void set_subint (const Integration*) override;

  //! Return the weight of the specified channel of the template sub-integration
  unsigned get_weight (unsigned ichan) override;

  //! return the reduced chisq for the current isubint, ichan, and ipol
  /*! Defined by the off-pulse noise in the template profile and provided profile. */
  double chisq(const Profile*) override;
};

int main (int argc, char** argv) try
{
  Pulsar::Profile::default_duty_cycle = 0.10;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of the archive containing the standard
  char* std_filename = NULL;

  // write the difference to stdout
  bool plot = false;
  float plot_when = 0;

  // perform likelihood analysis
  bool likelihood = false;

  char c;
  while ((c = getopt(argc, argv, "c:dfhLM:pqs:tXvV")) != -1) 

    switch (c)
    {
    case 'c':
      plot_when = atof(optarg);
      break;

    case 'd':
      plot = true;
      break;

    case 'f':
      fscrunch = true;
      break;

    case 'h':
      usage();
      return 0;

    case 'L':
      likelihood = true;
      break;
      
    case 'M':
      metafile = optarg;
      break;

    case 'p':
      pscrunch = true;
      break;

    case 's':
      std_filename = optarg;
      break;

    case 't':
      tscrunch = true;
      break;

    case 'X':
      print_reduced_chisq = true;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity (2);
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    } 


  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
  {
    cerr << "psrdiff: please specify filenames of files to be diffed" << endl;
    return -1;
  }

  if (likelihood)
    return likelihood_analysis (filenames);
  
  if (!std_filename)
  {
    if (filenames.size() == 2)
    {
      diff_two (filenames[0], filenames[1]);
      return 0;
    }

    cerr << "psrdiff: please specify standard (-s std.ar)" << endl;
    return -1;
  } 

  Reference::To<Pulsar::Archive> std_archive = Pulsar::Archive::load( std_filename );

  unsigned std_nsub = std_archive->get_nsubint();
  unsigned std_nchan = std_archive->get_nchan();
  unsigned std_npol = std_archive->get_npol();
  unsigned std_nbin = std_archive->get_nbin();

  if (std_nsub > 1)
    cerr << "psrdiff: warning! standard has more than one subint" << endl;

  std_archive->convert_state (Signal::Stokes);
  std_archive->remove_baseline ();

  Pulsar::PolnProfileFit fit;
  fit.choose_maximum_harmonic = true;

  if (verbose)
    fit.set_fit_debug();

  MEAL::Polar* polar = new MEAL::Polar;

  fit.set_transformation( polar );

  Pulsar::StokesPlot splot;

  if (plot || plot_when)
  {
    cpgopen ("?");
    cpgsvp (0.1, 0.9, 0.15, 0.9);
    cpgask (1);
  }

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load( filenames[ifile] );

    unsigned nsub = archive->get_nsubint();
    unsigned nchan = archive->get_nchan();
    unsigned npol = archive->get_npol();
    unsigned nbin = archive->get_nbin();

    if (nbin != std_nbin)
    {
      cerr << "psrdiff: " << archive->get_filename() << "\n    nbin=" << nbin
            << " != standard nbin=" << std_nbin << endl;
      continue;
    }

    if (nchan != std_nchan)
    {
      cerr << "psrdiff: " << archive->get_filename() << "\n    nchan=" << nchan
            << " != standard nchan=" << std_nchan << endl;
      continue;
    }

    if (npol != std_npol)
    {
      cerr << "psrdiff: " << archive->get_filename() << "\n    npol=" << npol
            << " != standard npol=" << std_npol << endl;
      continue;
    }

    archive->convert_state (Signal::Stokes);
    archive->remove_baseline ();

    Pulsar::Integration* std_subint = std_archive->get_Integration(0);

    unsigned isub=0;

    double total_chisq = 0.0;
    unsigned count = 0;

    for (isub=0; isub < nsub; isub++)
    {
      Pulsar::Integration* subint = archive->get_Integration(isub);

      vector<float> fit_chisq (nchan, 0.0);

      cerr << "psrdiff: performing fit in " << nchan << " channels" << endl;

      for (unsigned ichan=0; ichan < nchan; ichan++) try
      {
        if (std_subint->get_weight(ichan) == 0 || subint->get_weight(ichan) == 0)
          continue;

        Reference::To<Pulsar::PolnProfile> std_profile;
        std_profile = std_subint->new_PolnProfile (ichan);

        Reference::To<Pulsar::PolnProfile> profile;
        profile = subint->new_PolnProfile (ichan);
        
        // polar->set_rotationEuler (0, M_PI/2);

        fit.set_standard (std_profile);
        fit.fit (profile);
        
        unsigned nfree = fit.get_equation()->get_solver()->get_nfree ();
        float chisq = fit.get_equation()->get_solver()->get_chisq ();

        float reduced_chisq = chisq / nfree;

        if (verbose)
          cerr << ichan << " REDUCED CHISQ=" << reduced_chisq << endl;

        fit_chisq[ichan] = reduced_chisq;

        total_chisq += reduced_chisq;
        count ++;

        bool plot_this = (plot || (plot_when && reduced_chisq > plot_when));

        Estimate<double> pulse_phase = fit.get_phase ();

        MEAL::Complex2* xform = fit.get_transformation();

        if (verbose)
          for (unsigned i=0; i<xform->get_nparam(); i++)
            cerr << i << ":" << xform->get_param_name(i) << "=" 
                 << xform->get_Estimate(i) << endl;

        Jones<double> transformation = xform->evaluate();
        
        if (verbose)
          cerr << ichan << " shift=" << pulse_phase << endl;

        profile->rotate_phase (pulse_phase.get_value());
        profile->transform (inv(transformation));

        if (plot_this)
        {
          profile->diff(std_profile);

          Pulsar::Profile::rotate_in_phase_domain = false;
          profile->rotate_phase (.5);

          cpgpage();
          splot.set_subint (isub);
          splot.set_chan (ichan);
          splot.plot(archive);
        }

      }
      catch (Error& error) {
        cerr << error << endl;
      }

    }

    total_chisq /= count;

    TemplateDifference diff;
    diff.set_template(std_subint);

    double alt_chisq = diff.reduced_chisq(archive);

    cout << archive->get_filename() << " " << total_chisq << " " << alt_chisq << endl;
    
  }
  catch (Error& error) {
    cerr << "Error while handling '" << filenames[ifile] << "'" << endl << error.get_message() << endl;
  }

  if (plot || plot_when)
    cpgend();

  return 0;

}
catch (Error& error)
{
  cerr << "psrdiff: error" << error << endl;
  return -1;
}



class NumericDifference : public Difference
{
  Reference::To<const Archive> difference;
  double tolerance = pow(2.0,-16); // assume 16-bit archives

public:

  //! Set the difference between the data and another archive
  void set_difference(const Archive* _diff) { difference = _diff; }

  //! return the reduced chisq for the current isubint, ichan, and ipol
  /*! Defined by children. */
  double chisq(const Profile*) override;
};

void diff_two (const std::string& fileA, const std::string& fileB)
{
  Reference::To<Pulsar::Archive> A = Pulsar::Archive::load (fileA);
  Reference::To<Pulsar::Archive> B = Pulsar::Archive::load (fileB);

  if (fscrunch)
  {
    unsigned nchanA = A->get_nchan();
    unsigned nchanB = B->get_nchan();

    if (nchanA > nchanB)
      A->fscrunch_to_nchan (nchanB);
    if (nchanB > nchanA)
      B->fscrunch_to_nchan (nchanA);

    if (nchanA != nchanB)
    {
      /* if either of the above two cases are true, then it may be
         necessary to align the profiles to the arrival time of the
         header centre frequency in order to compensate for any
         mismatch in weighted centre frequencies */
      A->dedisperse ();
      B->dedisperse ();
    }
  }

  std::string reason;
  if (!A->mixable(B,reason))
  {
    cout << "files differ as follows:" << reason << endl;
    return;
  }

  if (verbose)
    cerr << "psrdiff: subtracting " << fileB << " from " << fileA << endl;

  unsigned nsub = A->get_nsubint();
  unsigned nchan = A->get_nchan();
  unsigned npol = A->get_npol();
  
  for (unsigned isub=0; isub < nsub; isub++)    
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
        Pulsar::Profile* profileA = A->get_Profile (isub, ipol, ichan);
        Pulsar::Profile* profileB = B->get_Profile (isub, ipol, ichan);

        profileA->diff(profileB);
      }

  if (print_reduced_chisq)
  {
    NumericDifference diff;
    diff.set_difference(A);

    cout << "Reduced chisq: " << diff.reduced_chisq(B) << endl;
    return;
  }

  std::string output_filename = "psrdiff.out";
  cerr << "psrdiff: unloading " << output_filename << endl;
  A->unload (output_filename);
}

double Difference::reduced_chisq(const Archive* archive)
{
  const unsigned nsubint = archive->get_nsubint();
  const unsigned nchan = archive->get_nchan();
  const unsigned npol = archive->get_npol();
  
  double total_chisq = 0.0;
  unsigned count = 0;

  for (isubint=0; isubint < nsubint; isubint++)
  {
    const Pulsar::Integration* subint = archive->get_Integration(isubint);
    set_subint(subint);

    for (ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0 || get_weight(ichan) == 0)
        continue;

      for (ipol=0; ipol < npol; ipol++)
      {
        const Profile* profile = subint->get_Profile (ipol,ichan);
        double chi2 = chisq(profile);

        if (verbose)
          cout << ichan << " ipol=" << ipol << " chisq=" << chi2 << endl;

        total_chisq += chi2;
        count ++;
      }
    }
  }

  return total_chisq / count;
}

//! Set the template to which the data will be compared
void TemplateDifference::set_template(const Integration* _template) 
{ 
  std_subint = _template;
  std_subint->baseline_stats (0, &std_variance);
}

unsigned TemplateDifference::get_weight(unsigned ichan)
{
  return std_subint->get_weight(ichan);
}

//! Perform any setup for the current sub-integration
void TemplateDifference::set_subint (const Integration* subint)
{
  subint->baseline_stats (0, &variance);
}

//! return the reduced chisq for the current isubint, ichan, and ipol
/*! Defined by the off-pulse noise in the template profile and provided profile. */
double TemplateDifference::chisq(const Profile* profile)
{
  double var = variance[ipol][ichan] + std_variance[ipol][ichan];

  const float* amps = profile->get_amps();
  const unsigned nbin = profile->get_nbin();

  const float* std_amps = std_subint->get_Profile(ipol,ichan)->get_amps();

  double diff = 0;
  for (unsigned ibin = 0; ibin < nbin; ibin++)
  {
    double val = amps[ibin] - std_amps[ibin];
    diff += val * val;
  }

  return diff / (nbin * var);
}

//! return the reduced chisq for the current isubint, ichan, and ipol
/*! Defined by children. */
double NumericDifference::chisq(const Profile* profile)
{
  const float* diff = difference->get_Profile(isubint,ipol,ichan)->get_amps();
  const unsigned nbin = profile->get_nbin();

  double range = profile->max() - profile->min();

  // cerr << "NumericDifference::chisq isubint=" << isubint << " ichan=" << ichan << " ipol=" << ipol << " range=" << range << endl;

  double error = tolerance * range;
  double var = 1.0;

  if (error > 0)
    var = error * error;

  double total_diff = 0;
  for (unsigned ibin = 0; ibin < nbin; ibin++)
  {
    double val = diff[ibin];
    total_diff += val * val;
  }

  double chi2 = total_diff / (nbin * var);
  
  // cerr << "NumericDifference::chisq tolerance=" << tolerance << " chisq=" << chi2 << endl;

  return chi2;
}


#if HAVE_ARMADILLO
#include "Pulsar/ArchiveComparisons.h"
#include "GaussianMixtureProbabilityDensity.h"
#include "GeneralizedChiSquared.h"
#include "UnaryStatistic.h"

using namespace BinaryStatistics;
using namespace Pulsar;

vector<double> log_likelihood (Archive* data, ArchiveComparisons* model)
{
  model->set_Archive (data);

  vector<double> result;

  unsigned nsubint = data->get_nsubint();
  unsigned nchan = data->get_nchan();

  // cerr << "nsubint=" << nsubint << endl;
  // cerr << "nchan=" << nchan << endl;
    
  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    Integration* subint = data->get_Integration( isubint );
    model->set_subint (isubint);

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
        continue;

      model->set_chan (ichan);
      result.push_back (model->get());
    }
  }

  return result;
}

#endif

int likelihood_analysis (vector<string>& filenames)
{

#if HAVE_ARMADILLO

  unsigned nfile = filenames.size();
  
  vector< Reference::To<ArchiveComparisons> > compare ( nfile );
  vector< GaussianMixtureProbabilityDensity* > gmpd ( nfile );
  vector< GeneralizedChiSquared* > gcs ( nfile );

  vector< Reference::To<Archive> > data ( nfile );

  unsigned ibest = 0;
  unsigned min_ngaus = 0;
  
  for (unsigned ifile=0; ifile < nfile; ifile++)
  {
    gmpd[ifile] = new GaussianMixtureProbabilityDensity;
    gmpd[ifile]->gcs = gcs[ifile] = new GeneralizedChiSquared;

    compare[ifile] = new ArchiveComparisons (gmpd[ifile]);
    compare[ifile]->set_what ("sum");
    compare[ifile]->set_way ("all");

    data[ifile] = Archive::load( filenames[ifile] );

    data[ifile]->bscrunch_to_nbin (128);
    data[ifile]->pscrunch();
    
    compare[ifile]->set_setup_Archive( data[ifile] );

    unsigned ngaus = gmpd[ifile]->get_ngaus ();
    
    cerr << "file=" << data[ifile]->get_filename() << endl;
    cerr << "neigen=" << gcs[ifile]->get_neigen () << endl;
    cerr << "ngaus=" << ngaus << endl;

    if (ifile == 0 || ngaus < min_ngaus)
    {
      min_ngaus = ngaus;
      ibest = ifile;
    }
  }

  for (unsigned ifile=0; ifile < nfile; ifile++)
  {
    cerr << endl << "****************************************" << endl;
    cerr << "BASIS: " << data[ifile]->get_filename() << endl;
    
    for (unsigned jfile=0; jfile < nfile; jfile++)
    {
      cerr << "TEST: " << data[jfile]->get_filename() << endl;

      vector<double> logL = log_likelihood (data[jfile], compare[ifile]);
      cerr << "N=" << logL.size() << endl;
      cerr << "mean log(L)=" << mean(logL) << endl;
      cerr << "median log(L)=" << median(logL) << endl;
    }
  }

  cerr << endl << "****************************************" << endl;
  cerr << "RESULTS:" << endl << endl;
  
  for (unsigned jfile=0; jfile < nfile; jfile++)
    cout << data[jfile]->get_filename() << " "
	 << mean(log_likelihood (data[jfile], compare[ibest])) << endl;

  return 0;
  
#else

  cerr << "psrdiff: ARMADILLO library required to perform likelihood analysis"
       << endl;

  return -1;
  
#endif

}
