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
    " -a archive set the output archive class name\n"
    " -c ICHAN   mark ICHAN as bad\n"
    " -f         treat all archives as members of a fluxcal observation\n"
    " -q         use the single-axis model\n"
    " -P         produce publication-quality plots" << endl;
}

int main (int argc, char** argv) 
{
  // use the Single Axis model
  bool single_axis = false;

  // treat all of the Archives as one FluxCalibrator observation set
  bool flux_cal = false;

  // filename of filenames
  char* metafile = NULL;

  // class name of the special calibrator solution archives to be produced
  string archive_class = "FITSArchive";

  // vector of bad channels
  vector<unsigned> zapchan;

  // produce publication quality plots
  bool publication = false;

  // verbosity flag
  bool verbose = false;
  char c;
  while ((c = getopt(argc, argv, "a:c:hfMPqvV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'a':
      archive_class = optarg;
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
	cerr << "pacv: Error parsing " << optarg << " as zap range" << endl;
	return -1;
      }

      break;
    }
      
    case 'f':
      flux_cal = true;
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'P':
      publication = true;
      break;

    case 'q':
      single_axis = true;
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
  
  // the input calibrator archive
  Reference::To<Pulsar::Archive> input;
  // the output calibrator archive
  Reference::To<Pulsar::Archive> output;
 
  Reference::To<Pulsar::PolnCalibrator> calibrator;
  
  Pulsar::CalibratorPlotter plotter;

  if (publication) {
    plotter.npanel = 5;
    plotter.between_panels = 0.08;
    cpgsvp (.25,.75,.15,.95);
    cpgslw (2);
  }

  Pulsar::FluxCalibrator fluxcal;
  Pulsar::Plotter archplot;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try {

    if (verbose)
      cerr << "pacv: Loading " << filenames[ifile] << endl;

    input = Pulsar::Archive::load( filenames[ifile] );


    if (input->get_type() == Signal::Calibrator) {

      calibrator = new Pulsar::PolnCalibrator (input);

      cerr << "pacv: Archive Calibrator with nchan=" 
	   << calibrator->get_nchan() << endl;

      for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	calibrator->set_Transformation_invalid (zapchan[ichan]);

      if (verbose)
	cerr << "pacv: Plotting PolnCalibrator" << endl;
      
      cpgpage ();
      plotter.plot (calibrator);

      continue;

    }



    for (unsigned ichan=0; ichan<zapchan.size(); ichan++) {
      if (verbose)
	cerr << "pacv: Zapping channel " << zapchan[ichan] << endl;

      for (unsigned isub=0; isub<input->get_nsubint(); isub++)
	input->get_Integration(isub)->set_weight (zapchan[ichan], 0.0);

    }


    if (flux_cal) {
      if (verbose)
	cerr << "pacv: Adding Archive to FluxCalibrator" << endl;
      
      fluxcal.add_observation (input);
      continue;
    }

    if (verbose)
      cerr << "pacv: Plotting Uncalibrated Spectrum" << endl;
    cpgpage ();
    archplot.calibrator_spectrum (input);

    if (verbose)
      cerr << "pacv: Constructing PolnCalibrator" << endl;

    if (single_axis)
      calibrator = new Pulsar::SingleAxisCalibrator (input);

    else
      calibrator = new Pulsar::PolarCalibrator (input);
    
    if (verbose)
      cerr << "pacv: Calibrating Archive" << endl;

    calibrator -> calibrate (input);

    if (verbose)
      cerr << "pacv: Plotting PolnCalibrator" << endl;

    cpgpage ();
    plotter.plot (calibrator);

    if (verbose)
      cerr << "pacv: Plotting Calibrated Spectrum" << endl;
    cpgpage ();
    archplot.calibrator_spectrum (input);


    if (!flux_cal) {

      cerr << "pacv: Creating " << archive_class << endl;
  
      output = calibrator->get_solution (archive_class);

      int index = filenames[ifile].find_first_of(".", 0);
      string newname = filenames[ifile].substr(0, index) + ".pacv";

      cerr << "pacv: Unloading " << newname << endl;
      output -> unload (newname);

    }

  }
  catch (Error& error) {
    cerr << "pacv: Error during " << filenames[ifile] << error << endl;
    return -1;
  }

  if (flux_cal) {
    cerr << "pacv: Plotting FluxCalibrator" << endl;
    plotter.plot (&fluxcal);
  }

  cpgend();
  return 0;
}
