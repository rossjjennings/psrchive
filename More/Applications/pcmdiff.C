/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#define PGPLOT 1

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/CalibratorPlotter.h"
#include "Pulsar/CalibratorStokesInfo.h"

#include "MEAL/Polar.h"

#include "strutil.h"
#include "dirutil.h"

#include <cpgplot.h>

#include <iostream>
#include <unistd.h>

using namespace std;

void usage ()
{
  cerr << "pcmdiff - Polarimetric Calibration Modelling Differences\n"
    "usage: pcmdiff -s pcm.std [options] file1 [file2 ...] \n"
    "where:\n"
    " -c [i|j-k]   mark channel or range of channels as bad \n"
    " -D dev       display using PGPLOT device \n"
    " -P           produce publication-quality plots\n"
    " -s pcm.std   compare all solutions with this standard\n" << endl;
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

  if (!archive->get_type() == Signal::Calibrator) {
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

  char c;
  while ((c = getopt(argc, argv, "c:D:hM:Ps:vV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

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

    case 'M':
      metafile = optarg;
      break;

    case 's':
      compare = load_calibrator (optarg, zapchan);
      compare_stokes = calibrator_stokes;
      break;

    case 'P':
      publication = true;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::CalibratorPlotter::verbose = true;
      Pulsar::Calibrator::verbose = true;
      //Calibration::Model::verbose = true;
    case 'v':
      verbose = true;
      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "pcmdiff requires a list of archive filenames as parameters.\n";
    return -1;
  }

  if (!compare) {
    cerr << "pcmdiff requires a standard (-s)\n";
    return -1;
  }

  vector <string> filenames;
  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  cpgbeg (0, device.c_str(), 0, 0);
  cpgask(1);
  cpgsvp (.1,.9, .1,.9);
 
  Pulsar::CalibratorPlotter plotter;

  if (publication) {
    plotter.npanel = 5;
    plotter.between_panels = 0.08;
    cpgsvp (.25,.75,.10,.90);
    cpgslw (2);
  }

  Reference::To<Pulsar::PolnCalibrator> calibrator;


  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try {

    calibrator = load_calibrator (filenames[ifile], zapchan);

    unsigned nchan = std::min (compare->get_nchan(), calibrator->get_nchan());

    unsigned comfac = compare->get_nchan() / nchan;
    unsigned calfac = calibrator->get_nchan() / nchan;

    for (unsigned ichan=0; ichan < nchan; ichan++) {

      unsigned calchan = ichan * calfac;
      unsigned comchan = ichan * comfac;

      // cerr << "calchan=" << calchan << " comchan=" << comchan << endl;

      if (!compare->get_transformation_valid(comchan)) {
	calibrator->set_transformation_invalid(calchan);
	calibrator_stokes->set_valid(calchan, false);
      }

      if (!calibrator->get_transformation_valid(calchan))
	continue;

      MEAL::Complex2* cal = calibrator->get_transformation(calchan);
      MEAL::Complex2* com = compare->get_transformation(comchan);

      unsigned nparam = com -> get_nparam();
      if (cal->get_nparam() != nparam) {
	cerr << "pcmdiff: calibrator nparam=" << cal->get_nparam() 
	     << " != " << nparam << endl;
	return -1;
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

    if (calfac) {
      // zap the extra channels
      cerr << "Zapping calfac=" << calfac << endl;
      for (unsigned ichan=0; ichan < nchan; ichan++)
	for (unsigned ifac=1; ifac < calfac; ifac++) {
	  calibrator->set_transformation_invalid(ichan*calfac+ifac);
	  calibrator_stokes->set_valid(ichan*calfac+ifac, false);
	}
    }

    cpgpage ();
    plotter.plot (calibrator);


    cpgpage ();
    plotter.plot( new Pulsar::CalibratorStokesInfo (calibrator_stokes),
		  calibrator->get_nchan(),
		  calibrator->get_Archive()->get_centre_frequency(),
		  calibrator->get_Archive()->get_bandwidth() );

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


