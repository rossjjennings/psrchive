/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

#if HAVE_PGPLOT
# include "Pulsar/CalibratorPlotter.h"
# include "Pulsar/CalibratorStokesInfo.h"
# include <cpgplot.h>
#endif

#include "MEAL/Polar.h"

#include "strutil.h"
#include "dirutil.h"
#include "pairutil.h"

#include <iostream>
#include <unistd.h>

using namespace std;

void usage ()
{
  cerr << "pcmdiff - Polarimetric Calibration Modelling Differences\n"
    "usage: pcmdiff -s pcm.std [options] file1 [file2 ...] \n"
    "where:\n"
    " -c [i|j-k]   mark channel or range of channels as bad \n"
    " -C           don't plot calibrator Stokes \n"
    " -D dev       display using PGPLOT device \n"
    " -P           produce publication-quality plots \n"
    " -E           plot without error bars \n"
    " -S i:j       swap parameter i and j before comparing \n"
    " -s pcm.std   compare all solutions with this standard \n"
    " -p           print statistical summary of differences (no plot) \n"
    " -A           print the relative likelihood (Akaike weight) \n"
    " \n"
       << endl;
}


// verbosity flag
static bool verbose = false;

Reference::To<Pulsar::Archive> archive;
Reference::To<Pulsar::CalibratorStokes> calibrator_stokes;

Pulsar::PolnCalibrator* load_calibrator (const string& filename,
					 const vector<unsigned>& zapchan)
{
  cerr << "pcmdiff: Loading " << filename << endl;

  archive = Pulsar::Archive::load( filename );

  if (archive->get_type() != Signal::Calibrator) {
    cerr << "pcmdiff: Archive " << filename << " is not a Calibrator" << endl;
    return 0;
  }

  Reference::To<Pulsar::PolnCalibrator> calibrator;

  calibrator = new Pulsar::PolnCalibrator (archive);

  if (verbose)
    cerr << "pcmdiff: Archive Calibrator with nchan=" 
	 << calibrator->get_nchan() << endl;

  unsigned ichan = 0;

  for (ichan=0; ichan<zapchan.size(); ichan++)
    calibrator->set_transformation_invalid (zapchan[ichan]);

  calibrator_stokes = archive->get<Pulsar::CalibratorStokes>();
    
  for (ichan=0; ichan<zapchan.size(); ichan++)
    calibrator_stokes->set_valid (zapchan[ichan], false);

  return calibrator.release();
}

typedef std::pair<unsigned,unsigned> ipair;

double get_chisq (const vector<Estimate<double> >& residuals)
{
  unsigned ndat = residuals.size();
  double sumsq = 0.0;
  
  for (unsigned idat=0; idat < ndat; idat++)
  {
    double val = residuals[idat].val;
    double var = residuals[idat].var;
    sumsq += val*val / var;
  }
  
  return sumsq / ndat;  
}

double get_max_outlier (const vector<Estimate<double> >& residuals)
{
  unsigned ndat = residuals.size();
  double max = 0.0;
  
  for (unsigned idat=0; idat < ndat; idat++)
  {
    double val = residuals[idat].val;
    double var = residuals[idat].var;
    double out = val*val / var;

    if (out > max)
      max = out;
  }

  return sqrt(max);
}

double get_AIC (const Pulsar::PolnCalibrator* calibrator)
{
  unsigned nchan  = calibrator->get_nchan ();

  double total_chisq = 0.0;
  unsigned total_ndat = 0;
  unsigned total_nfit = 0;
  
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!calibrator->get_transformation_valid(ichan))
      continue;

    const MEAL::LeastSquares* solver = calibrator->get_solver (ichan);

    total_chisq += solver->get_chisq ();

    unsigned nfree = solver->get_nfree ();
    unsigned nfit = solver->get_nparam_infit ();
    
    total_nfit += nfit;
    total_ndat += nfit + nfree;
  }

  return total_chisq + 2.0 * total_nfit
    + 2.0 * total_nfit * (total_nfit+1) / (total_ndat - total_nfit - 1.0); 
}
	     
double get_relative_likelihood (Pulsar::PolnCalibrator* A, Pulsar::PolnCalibrator* B)
{
  double AIC_A = get_AIC (A);
  double AIC_B = get_AIC (B);

  cerr << "AIC_A=" << AIC_A << " AIC_B=" << AIC_B << endl;
  
  return exp( -0.5 * (AIC_A - AIC_B) );
}

int main (int argc, char** argv) try
{
  // filename of filenames
  char* metafile = NULL;

  // vector of bad channels
  vector<unsigned> zapchan;

  // produce publication quality plots
  bool publication = false;

  // plot device
  string device = "?";

  Reference::To<Pulsar::PolnCalibrator> compare;
  Reference::To<Pulsar::CalibratorStokes> compare_stokes;

  bool plot_errors = true;
  bool plot_calibrator_stokes = true;
  
  bool print_summary = false;
  bool print_relative_likelihood = false;
  bool print_only = false;
  
  vector<ipair> swaps;
  
  char c;
  while ((c = getopt(argc, argv, "ACc:D:EhM:pPs:S:vV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'A':
      print_relative_likelihood = true;
      print_only = true;
      break;
      
    case 'C':
      plot_calibrator_stokes = false;
      break;

    case 'c': {

      unsigned ichan1 = 0;
      unsigned ichan2 = 0;

      if (sscanf (optarg, "%u-%u", &ichan1, &ichan2) == 2)
	for (unsigned ichan=ichan1; ichan<=ichan2; ichan++)
	  zapchan.push_back(ichan);

      else if (sscanf (optarg, "%u", &ichan1) == 1)
	zapchan.push_back(ichan1);

      else {
	cerr << "pcmdiff: Error parsing " << optarg << " as zap range" << endl;
	return -1;
      }

      break;
    }

    case 'D':
      device = optarg;
      break;

    case 'E':
      plot_errors = false;
      break;
      
    case 'M':
      metafile = optarg;
      break;

    case 's':
      compare = load_calibrator (optarg, zapchan);
      compare_stokes = calibrator_stokes;
      break;

    case 'S':
      swaps.push_back( fromstring<ipair>(optarg) );
      break;
      
    case 'P':
      publication = true;
      break;

    case 'p':
      print_summary = true;
      print_only = true;
      break;
      
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::Calibrator::verbose = true;
      //Calibration::Model::verbose = true;

#if HAVE_PGPLOT
      Pulsar::CalibratorPlotter::verbose = true;
#endif

    case 'v':
      verbose = true;
      break;

    } 
  }

  if (!metafile && optind >= argc)
  {
    cerr << "pcmdiff requires a list of archive filenames as parameters.\n";
    return -1;
  }

  if (!compare)
  {
    cerr << "pcmdiff requires a standard (-s)\n";
    return -1;
  }

  vector <string> filenames;
  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

#if HAVE_PGPLOT
  Pulsar::CalibratorPlotter plotter;

  if (!print_only)
  {
    cpgbeg (0, device.c_str(), 0, 0);
    cpgask(1);
    cpgsvp (.1,.9, .1,.9);
  
    plotter.plot_error_bars = plot_errors;
    if (publication)
    {
      plotter.npanel = 5;
      plotter.between_panels = 0.08;
      cpgsvp (.25,.75,.10,.90);
      cpgslw (2);
    }
  }
#endif
  
  Reference::To<Pulsar::PolnCalibrator> calibrator;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    calibrator = load_calibrator (filenames[ifile], zapchan);

    unsigned nchan = std::min (compare->get_nchan(), calibrator->get_nchan());
    unsigned nparam = 0;
    unsigned comfac = compare->get_nchan() / nchan;
    unsigned calfac = calibrator->get_nchan() / nchan;

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      unsigned calchan = ichan * calfac;
      unsigned comchan = ichan * comfac;

      // cerr << "calchan=" << calchan << " comchan=" << comchan << endl;

      if (!compare->get_transformation_valid(comchan))
      {
	calibrator->set_transformation_invalid(calchan);
	calibrator_stokes->set_valid(calchan, false);
      }

      if (!calibrator->get_transformation_valid(calchan))
	continue;

      MEAL::Complex2* cal = calibrator->get_transformation(calchan);
      MEAL::Complex2* com = compare->get_transformation(comchan);

      nparam = com -> get_nparam();
      if (cal->get_nparam() != nparam) {
	cerr << "pcmdiff: calibrator nparam=" << cal->get_nparam() 
	     << " != " << nparam << endl;
	return -1;
      }

      for (unsigned iswap=0; iswap < swaps.size(); iswap++)
      {
	unsigned a = swaps[iswap].first;
	unsigned b = swaps[iswap].second;
	
	Estimate<double> tmp = cal->get_Estimate(a);
	cal->set_Estimate(a,cal->get_Estimate(b));
	cal->set_Estimate(b,tmp);
      }
      
      for (unsigned iparam=0; iparam < nparam; iparam++)
	cal->set_Estimate( iparam,
			   cal->get_Estimate(iparam) -
			   com->get_Estimate(iparam) );

      calibrator_stokes->set_stokes
	( calchan,
	  calibrator_stokes->get_stokes(calchan) -
	  compare_stokes->get_stokes(comchan) );
    }

    if (calfac > 1)
    {
      // zap the extra channels
      cerr << "Zapping calfac=" << calfac << endl;
      for (unsigned ichan=0; ichan < nchan; ichan++)
	for (unsigned ifac=1; ifac < calfac; ifac++) {
	  calibrator->set_transformation_invalid(ichan*calfac+ifac);
	  calibrator_stokes->set_valid(ichan*calfac+ifac, false);
	}
    }

    if (print_summary)
    {
      nchan = calibrator->get_nchan();

      vector< Estimate<double> > diff (nchan);
      
      for (unsigned iparam=0; iparam<nparam; iparam++)
      {
	unsigned have_chan = 0;

	for (unsigned ichan=0; ichan < nchan; ichan++)
	{
	  if (!calibrator->get_transformation_valid(ichan))
	    continue;

	  MEAL::Complex2* cal = calibrator->get_transformation(ichan);

	  diff[have_chan] = cal->get_Estimate(iparam);
	  have_chan++;
	}

	diff.resize (have_chan);

	double chisq = get_chisq (diff);
	double maxout = get_max_outlier (diff);
	
	cout << filenames[ifile]
	     << " iparam= " << iparam
	     << " chisq= " << chisq
	     << " maxout= " << maxout << endl;
      }

      diff.resize (nchan);

      nparam = 3;

      for (unsigned iparam=0; iparam<nparam; iparam++)
      {
	unsigned have_chan = 0;

	for (unsigned ichan=0; ichan < nchan; ichan++)
	{
	  if (!calibrator_stokes->get_valid(ichan))
	    continue;

	  Stokes< Estimate<double> > S = calibrator_stokes->get_stokes (ichan);

	  diff[have_chan] = S[iparam+1];
	  have_chan++;
	}

	diff.resize (have_chan);

	double chisq = get_chisq (diff);
	double maxout = get_max_outlier (diff);
	
	cout << filenames[ifile]
	     << " ical= " << iparam
	     << " chisq= " << chisq
	     << " maxout= " << maxout << endl;
      }
    }
    else if (print_relative_likelihood)
    {
      double relative_likelihood = get_relative_likelihood (compare, calibrator);
      cout << filenames[ifile] << " l= " << relative_likelihood << endl;
    }
#if HAVE_PGPLOT
    else
    {
      cpgpage ();
      plotter.plot (calibrator);
      
      if (plot_calibrator_stokes)
      {
	cpgpage ();
	plotter.plot( new Pulsar::CalibratorStokesInfo (calibrator_stokes),
		      calibrator->get_nchan(),
		      calibrator->get_Archive()->get_centre_frequency(),
		      calibrator->get_Archive()->get_bandwidth() );
      }
    }
#endif
    
  }
  catch (Error& error) {
    cerr << "pcmdiff: Error during " << filenames[ifile] << error << endl;
    return -1;
  }

  return 0;
}
catch (Error& error) {
  cerr << "pcmdiff: error" << error << endl;
  return -1;
}


