#define PGPLOT 1

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"

#include "Pulsar/CalibratorPlotter.h"

#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Plotter.h"

#include "string_utils.h"
#include "dirutil.h"

#include <cpgplot.h>

#include <iostream>
#include <unistd.h>


void usage ()
{
  cerr << "pacv - Pulsar Archive Calibrator Viewer\n"
    "usage: pacv [options] file1 [file2 ...]\n"
    "where:\n"
    " -c ICHAN   mark ICHAN as bad\n"
    " -f         treat all archives as members of a fluxcal observation\n"
    " -q         use the single-axis model" << endl;
}

int main (int argc, char** argv) 
{
  // use the Single Axis model
  bool single_axis = false;

  // treat all of the Archives as one FluxCalibrator observation set
  bool flux_cal = false;

  // filename of filenames
  char* metafile = NULL;

  // vector of bad channels
  vector<unsigned> zapchan;

  // verbosity flag
  bool verbose = false;
  char c;
  while ((c = getopt(argc, argv, "c:hfMqvV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'c': {
      unsigned ichan = 0;
      if (sscanf (optarg, "%u", &ichan) != 1) {
	cerr << "pacv: Error parsing " << optarg << " as a channel" << endl;
	return -1;
      }
      zapchan.push_back(ichan);
      break;
    }
      
    case 'f':
      flux_cal = true;
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'q':
      single_axis = true;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::Calibrator::verbose = true;
    case 'v':
      verbose = true;
      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "pacv requires a list of archive filenames as parameters.\n";
    return -1;
  }

  vector <string> filenames;
  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  cpgbeg (0, "?", 0, 0);
  cpgask(1);

  cpgsvp (.1,.9, .1,.9);
  
  // the calibrator archive
  Reference::To<Pulsar::Archive> archive;
 
  Reference::To<Pulsar::PolnCalibrator> calibrator;
  
  Pulsar::CalibratorPlotter plotter;
  Pulsar::FluxCalibrator fluxcal;
  Pulsar::Plotter archplot;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {  try {

    if (verbose)
      cerr << "pacv: Loading " << filenames[ifile] << endl;

    archive = Pulsar::Archive::load( filenames[ifile] );

    for (unsigned ichan=0; ichan<zapchan.size(); ichan++) {
      if (verbose)
	cerr << "pacv: Zapping channel " << zapchan[ichan] << endl;

      for (unsigned isub=0; isub<archive->get_nsubint(); isub++)
	archive->get_Integration(isub)->set_weight (zapchan[ichan], 0.0);

    }


    if (flux_cal) {
      if (verbose)
	cerr << "pacv: Adding Archive to FluxCalibrator" << endl;
      
      fluxcal.add_observation (archive);
      continue;
    }

    if (verbose)
      cerr << "pacv: Plotting Uncalibrated Spectrum" << endl;
    cpgpage ();
    archplot.calibrator_spectrum (archive);

    if (verbose)
      cerr << "pacv: Constructing PolnCalibrator" << endl;

    if (single_axis)
      calibrator = new Pulsar::SingleAxisCalibrator (archive);

    else
      calibrator = new Pulsar::PolarCalibrator (archive);
    
    if (verbose)
      cerr << "pacv: Calibrating Archive" << endl;

    calibrator -> calibrate (archive);

    if (verbose)
      cerr << "pacv: Plotting PolnCalibrator" << endl;

    cpgpage ();
    plotter.plot (calibrator);

    if (verbose)
      cerr << "pacv: Plotting Calibrated Spectrum" << endl;
    cpgpage ();
    archplot.calibrator_spectrum (archive);


  } catch (Error& error) {
    cerr << "pacv: Error loading " << filenames[ifile] << error << endl;
    return -1;
  }
  }

  if (flux_cal) {
    cerr << "pacv: Plotting FluxCalibrator" << endl;
    plotter.plot (&fluxcal);
  }

  cpgend();
  return 0;
}
