#define PGPLOT 1

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorPlotter.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/SingleAxisCalibratorPlotter.h"

#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/PolarCalibratorPlotter.h"

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
    "usage: pacv [-q] file1 [file2 ...]" << endl;
}

int main (int argc, char** argv) 
{
  // ensure that parameters are stored for plotting
  Pulsar::PolnCalibrator::store_parameters = true;

  // use the Single Axis model
  bool single_axis = false;

  // treat all of the Archives as one FluxCalibrator observation set
  bool flux_cal = false;

  // filename of filenames
  char* metafile = NULL;

  // verbosity flag
  bool verbose = false;

  char c;
  while ((c = getopt(argc, argv, "hfMqvV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

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
  Reference::To<Pulsar::CalibratorPlotter> plotter;

  Pulsar::FluxCalibrator fluxcal;
  Pulsar::Plotter archplot;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {  try {

    if (verbose)
      cerr << "pacv: Loading " << filenames[ifile] << endl;

    archive = Pulsar::Archive::load( filenames[ifile] );

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

    if (single_axis) {
      calibrator = new Pulsar::SingleAxisCalibrator (archive);
      plotter = new Pulsar::SingleAxisCalibratorPlotter;
    }
    else {
      calibrator = new Pulsar::PolarCalibrator (archive);
      plotter = new Pulsar::PolarCalibratorPlotter;
    }

    
    if (verbose)
      cerr << "pacv: Calibrating Archive" << endl;

    calibrator -> calibrate (archive);

    if (verbose)
      cerr << "pacv: Plotting PolnCalibrator" << endl;

    cpgpage ();
    plotter->plot (calibrator);

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
    plotter = new Pulsar::FluxCalibratorPlotter;
    plotter->plot (&fluxcal);
  }

  cpgend();
  return 0;
}
