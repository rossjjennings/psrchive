#define PGPLOT 1

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/SingleAxisCalibratorPlotter.h"

#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/PolarCalibratorPlotter.h"

#include "Pulsar/Archive.h"

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
  // use the Single Axis model
  bool single_axis = false;
  // filename of filenames
  char* metafile = NULL;
  // verbosity flag
  bool verbose = false;

  char c;
  while ((c = getopt(argc, argv, "hMqvV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

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
      Error::verbose = true;
      verbose = true;
      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "psrcal requires a list of archive filenames as parameters.\n";
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

  cpgsvp (0.1,.95, 0.1,.95);
  
  // the calibrator archive
  Reference::To<Pulsar::Archive> archive;
 
  Reference::To<Pulsar::PolnCalibrator> calibrator;
  Reference::To<Pulsar::CalibratorPlotter> plotter;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {  try {

    if (verbose)
      cerr << "pacv: Loading " << filenames[ifile] << endl;

    archive = Pulsar::Archive::load( filenames[ifile] );

    if (verbose)
      cerr << "pacv: Constructing Calibrator" << endl;

    if (single_axis) {
      calibrator = new Pulsar::SingleAxisCalibrator (archive);
      plotter = new Pulsar::SingleAxisCalibratorPlotter;
    }
    else {
      calibrator = new Pulsar::PolarCalibrator (archive);
      plotter = new Pulsar::PolarCalibratorPlotter;
    }

    calibrator -> build();

    if (verbose)
      cerr << "pacv: Plotting Calibrator" << endl;

    plotter->plot (calibrator);

    cpgpage ();

  } catch (Error& error) {
    cerr << "psrcal: Error calibrating " << filenames[ifile] << error << endl;
    return -1;
  }
  }

  cpgend();
  return 0;
}
