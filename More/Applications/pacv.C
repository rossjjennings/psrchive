#define PGPLOT 1

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/DoPCalibrator.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "Calibration/Feed.h"

#include "Pulsar/CalibratorPlotter.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/CalibratorStokesInfo.h"

#include "Pulsar/Profile.h"
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
  cout << "pacv - Pulsar Archive Calibrator Viewer\n"
    "usage: pacv [options] file1 [file2 ...]\n"
    "where:\n"
    " -a archive   set the output archive class name\n"
    " -c [i|j-k]   mark channel or range of channels as bad\n"
    " -C           plot only calibrator Stokes\n"
    " -D dev       specify PGPLOT device\n"
    " -d           use the Degree of Polarization Calibrator\n"
    " -f           treat all archives as members of a fluxcal observation\n"
    " -p           use the polar model\n"
    " -P           produce publication-quality plots\n"
    " -r feed.txt  set the feed transformation [not used]\n"
    " -2 m or d    multiply or divide cross products by factor of two\n"
       << endl;
}

void plotJones (Pulsar::PolnCalibrator* calibrator, unsigned interpolate);

int main (int argc, char** argv) 
{
  // use the Single Axis model
  bool single_axis = true;

  // use the Degree of Polarization Calibrator
  bool dop_calibrator = false;

  // treat all of the Archives as one FluxCalibrator observation set
  Reference::To<Pulsar::FluxCalibrator> fluxcal;

  // filename of filenames
  char* metafile = NULL;

  // class name of the special calibrator solution archives to be produced
  string archive_class = "PSRFITS";

  // vector of bad channels
  vector<unsigned> zapchan;

  // produce publication quality plots
  bool publication = false;

  bool plot_calibrator_stokes = false;

  unsigned test_interpolation = 0;

  //
  float cross_scale_factor = 1.0;

  // known feed transformation
  Calibration::Feed* feed = 0;

  string device = "?";

  // verbosity flag
  bool verbose = false;
  char c;
  while ((c = getopt(argc, argv, "2:a:c:CD:dfhi:MPr:pqvV")) != -1)  {

    switch (c)  {

    case '2':
      if (optarg[0] == 'm')
	cross_scale_factor = 2.0;
      else if (optarg[0] == 'd')
	cross_scale_factor = 0.5;
      else {
	cerr << " -2 " << optarg << " not recognized" << endl;
	return -1;
      }
      break;

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
      
    case 'C':
      plot_calibrator_stokes = true;
      break;

    case 'D':
      device = optarg;
      break;

    case 'd':
      dop_calibrator = true;
      break;

    case 'f':
      fluxcal = new Pulsar::FluxCalibrator;
      break;

    case 'i':
      test_interpolation = atoi(optarg);
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'P':
      publication = true;
      break;

    case 'p':
      single_axis = false;
      break;

    case 'r':
      feed = new Calibration::Feed;
      feed -> load (optarg);
      cerr << "pac: Feed parameters loaded:"
	"\n  orientation 0 = "
	   << feed->get_orientation(0).get_value() * 180/M_PI << " deg"
	"\n  ellipticity 0 = "
	   << feed->get_ellipticity(0).get_value() * 180/M_PI << " deg"
	"\n  orientation 1 = "
	   << feed->get_orientation(1).get_value() * 180/M_PI << " deg"
	"\n  ellipticity 1 = "
	   << feed->get_ellipticity(1).get_value() * 180/M_PI << " deg"
	   << endl;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::CalibratorPlotter::verbose = true;
      Pulsar::Calibrator::verbose = true;
      verbose = true;
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
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

  cpgbeg (0, device.c_str(), 0, 0);
  cpgask(1);
  cpgsvp (.1,.9, .1,.9);
  
  // the input calibrator archive
  Reference::To<Pulsar::Archive> input;
  // the output calibrator archive
  Reference::To<Pulsar::Archive> output;
 
  Reference::To<Pulsar::PolnCalibrator> calibrator;

  Reference::To<Pulsar::CalibratorStokes> calibrator_stokes;
  
  Pulsar::CalibratorPlotter plotter;

  if (publication) {
    plotter.npanel = 5;
    plotter.between_panels = 0.08;
    cpgsvp (.25,.75,.15,.95);
    cpgslw (2);
  }

  Pulsar::Plotter archplot;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try {

    if (verbose)
      cerr << "pacv: Loading " << filenames[ifile] << endl;

    input = Pulsar::Archive::load( filenames[ifile] );

    if (input->get_type() == Signal::Calibrator) {

      cerr << "pacv: " << filenames[ifile] << " is a processed Calibrator"
           << endl;

      if (input->get<Pulsar::FluxCalibratorExtension>()) {

        cerr << "pacv: constructing FluxCalibrator from Extension" << endl;
        fluxcal = new Pulsar::FluxCalibrator (input);

	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  fluxcal->set_invalid (zapchan[ichan]);

        cerr << "pacv: Plotting FluxCalibrator" << endl;
	cpgpage ();
        plotter.plot (fluxcal);

        // disable attempt to plot again after end of main loop
        fluxcal = 0;

        continue;
      }

      calibrator = new Pulsar::PolnCalibrator (input);

      cerr << "pacv: Archive Calibrator with nchan=" 
	   << calibrator->get_nchan() << endl;

      if (!plot_calibrator_stokes) {

	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  calibrator->set_transformation_invalid (zapchan[ichan]);
	
	if (verbose)
	  cerr << "pacv: Plotting PolnCalibrator" << endl;
	
	cpgpage ();
	plotter.plot (calibrator);

        calibrator_stokes = 0;

	if (test_interpolation)
	  plotJones (calibrator,test_interpolation);

      }
      else
        calibrator_stokes = input->get<Pulsar::CalibratorStokes>();

      if (calibrator_stokes) {

	cerr << "pacv: Plotting CalibratorStokes" << endl;

	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  calibrator_stokes->set_valid (zapchan[ichan], false);

	cpgpage ();
	plotter.plot( new Pulsar::CalibratorStokesInfo (calibrator_stokes),
		      calibrator->get_nchan(),
		      calibrator->get_Archive()->get_centre_frequency(),
		      calibrator->get_Archive()->get_bandwidth() );
	
      }

      continue;

    }

    for (unsigned ichan=0; ichan<zapchan.size(); ichan++) {
      if (verbose)
	cerr << "pacv: Zapping channel " << zapchan[ichan] << endl;

      for (unsigned isub=0; isub<input->get_nsubint(); isub++)
	input->get_Integration(isub)->set_weight (zapchan[ichan], 0.0);

    }

    if (cross_scale_factor != 1.0) {

      cerr << "Scaling cross products by " << cross_scale_factor << endl;

      for (unsigned isub=0; isub < input->get_nsubint(); isub++)
	for (unsigned ichan=0; ichan < input->get_nchan(); ichan++) {
	  input->get_Profile (isub, 2, ichan) -> scale (cross_scale_factor);
	  input->get_Profile (isub, 3, ichan) -> scale (cross_scale_factor);
	}

    }

    if (fluxcal) {
      if (verbose)
	cerr << "pacv: Adding Archive to FluxCalibrator" << endl;
      
      fluxcal->add_observation (input);
      continue;
    }

    if (verbose)
      cerr << "pacv: Plotting Uncalibrated Spectrum" << endl;
    cpgpage ();
    archplot.calibrator_spectrum (input);

    if (verbose)
      cerr << "pacv: Constructing PolnCalibrator" << endl;

    if (dop_calibrator)
      calibrator = new Pulsar::DoPCalibrator (input);
    else if (single_axis)
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

    input->convert_state (Signal::Stokes);
    archplot.calibrator_spectrum (input);

    if (dop_calibrator)
      continue;

    cerr << "pacv: Creating " << archive_class << " Archive" << endl;
  
    output = calibrator->new_solution (archive_class);

    int index = filenames[ifile].find_first_of(".", 0);
    string newname = filenames[ifile].substr(0, index) + ".pacv";
    
    cerr << "pacv: Unloading " << newname << endl;
    output -> unload (newname);

  }
  catch (Error& error) {
    cerr << "pacv: Error during " << filenames[ifile] << error << endl;
    return -1;
  }
  catch (...)  {
    cerr << "pacv: An unknown exception was thrown" << endl;
  }


  if (fluxcal) try {
    cerr << "pacv: Plotting FluxCalibrator" << endl;
    plotter.plot (fluxcal);

    cerr << "pacv: Creating " << archive_class << " Archive" << endl;

    output = fluxcal->new_solution (archive_class);

    cerr << "pacv: Creating new filename" << endl;
    int index = filenames[0].find_first_of(".", 0);
    string newname = filenames[0].substr(0, index) + ".fcal";

    cerr << "pacv: Unloading " << newname << endl;
    output -> unload (newname);
  }
  catch (Error& error) {
    cerr << "pacv: Error plotting FluxCalibrator" << error << endl;
    return -1;
  }

  cpgend();

  return 0;
}
