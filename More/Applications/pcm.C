//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pcm.C,v $
   $Revision: 1.31 $
   $Date: 2004/08/13 13:06:19 $
   $Author: straten $ */

/*! \file pcm.C 
    \brief (Pulsar) Polarimetric Calibration Modelling (Package)

    This program may be used to model the polarimetric response of observatory
    instrumentation using two different techniques.

    In Mode A, multiple observations of
    <UL>
    <LI> a single pulsar at multiple parallactic angles,
    <LI> the amplitude modulated linear noise diode, and
    <LI> an unpolarized flux calibrator, such as Hydra A,
    </UL>
    are used to model the reception of all three signals as described
    in van Straten (2004).

    In Mode B, single observations of
    <UL>
    <LI> a calibrated pulsar with high signal-to-noise ratio,
    <LI> the same pulsar, uncalibrated, and
    <LI> the amplitude modulated linear noise diode
    </UL>
    are used to model the instrumental response.

*/

#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/Database.h"

#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/SourceInfo.h"

#include "Pulsar/Plotter.h"
#include "Pulsar/Archive.h"
#include "Pulsar/getopt.h"

#include "RealTimer.h"
#include "Error.h"
#include "dirutil.h"
#include "string_utils.h"

#include <cpgplot.h>

#include <iostream>
#include <algorithm>
#include <unistd.h>

static string Britton = "Britton";
static string Hamaker = "Hamaker";

void usage ()
{
  cout << "A program for performing self-calibration \n"
    "Usage: pcm [options] [filenames] \n"
    "\n"
    "  -h         this help page \n"
    "  -V level   set verbosity level [0->4] \n"
    "  -a archive set the output archive class name \n"
    "  -m model   model: Britton [default] or Hamaker \n"
    "\n"
    "  -C meta    filename with list of calibrator files \n"
    "  -d dbase   filename of Calibration Database \n"
    "  -M meta    filename with list of pulsar files \n"
    "\n"
    "MODE A: Fit multiple observations of unknown source \n"
    "\n"
    "  -f chan    solve for only the specified channel \n"
    "  -t nproc   solve using nproc threads \n"
    "\n"
    "  -b bin     add phase bin to constraints \n"
    "  -n nbin    set the number of phase bins to choose as input states \n"
    "  -p pA,pB   set the phase window from which to choose input states \n"
    "  -c archive choose best input states from input archive \n"
    "\n"
    "  -s         do not normalize Stokes parameters by invariant interval \n"
    "\n"
    "  -q         assume that CAL Stokes Q = 0 \n"
    "  -v         assume that CAL Stokes V = 0 \n"
    "\n"
    "MODE B: Fit single observations of known source \n"
    "\n"
    "  -S fname   filename of calibrated standard \n"
    "  -n nbin    set the number of harmonics to use as input states \n"
       << endl;
}

void choose (vector<unsigned>& bins, Pulsar::Archive* archive);

void auto_select (Pulsar::ReceptionCalibrator& model,
		  Pulsar::Archive* archive,
		  unsigned maxbins)
{
  vector<unsigned> bins (maxbins);

  choose (bins, archive);

  sort (bins.begin(), bins.end());

  for (unsigned ibin=0; ibin < bins.size(); ibin++) {
    cerr << "pcm: adding phase bin " << bins[ibin] << endl;
    model.add_state (bins[ibin]);
  }
}


void range_select (Pulsar::ReceptionCalibrator& model,
		   Pulsar::Archive* archive,
		   float phmin, float phmax, unsigned maxbins)
{
  unsigned nbin = archive->get_nbin();

  float increment = (phmax - phmin)/maxbins;

  unsigned last_bin = unsigned (phmax * nbin);

  for (float bin = phmin; bin<=phmax; bin += increment) {

    unsigned ibin = unsigned (bin * nbin);

    if (ibin != last_bin)  {
      cerr << "pcm: adding phase bin " << ibin << endl;
      model.add_state (ibin%nbin);
      last_bin = ibin;
    }
  }
}

void plot_pulsar (Pulsar::ReceptionCalibratorPlotter& plotter,
		  Pulsar::ReceptionCalibrator& model)
{
  unsigned panels = plotter.npanel;
  plotter.npanel = 4;

  unsigned nstate = model.get_nstate_pulsar();
  double centre_frequency = model.get_Archive()->get_centre_frequency();
  double bandwidth = model.get_Archive()->get_bandwidth();

  Reference::To<Pulsar::SourceInfo> info;

  for (unsigned istate=0; istate<nstate; istate++) {

    info = new Pulsar::SourceInfo (&model, istate);

    cpgpage();
    plotter.plot (info, model.get_nchan(), centre_frequency, bandwidth);

  }

  plotter.npanel = panels;

}

void plot_constraints (Pulsar::ReceptionCalibratorPlotter& plotter,
		       unsigned nchan, unsigned nstate, unsigned nplot,
		       unsigned start_state, unsigned only_chan=0)
{
  unsigned incr = nchan/nplot;
  if (!incr)
    incr = 1;

  unsigned ichan = 1;

  if (nplot == 1)
    ichan = only_chan;

  if (nstate == 1)
    cpgbeg (0, "states.ps/CPS", 0, 0);

  for (; ichan < nchan; ichan+=incr) {

    if (nstate > 1) {
      char filename [256];
      sprintf (filename, "channel_%d.ps/CPS", ichan);
      cpgbeg (0, filename, 0, 0);
    }

    if (plotter.use_colour)
      cpgsvp (.15,.9, .15,.9);
    else
      cpgsvp (.25,.75,.15,.95);

    // cerr << "pcm: nstate=" << nstate << endl;
    for (unsigned istate=0; istate<nstate; istate++) {

      unsigned plot_state = istate+start_state;

      // cerr << "ichan=" << ichan << " istate=" << plot_state << endl;
      plotter.plot_psr_constraints (ichan, plot_state);

      if (istate+1 < nstate)
	cpgpage();
    }

    if (nstate > 1)
      cpgend ();
    else if (ichan+incr < nchan)
      cpgpage ();

  }

  if (nstate == 1)
    cpgend ();

}

int mode_B (const char* standard_filename,
	    const vector<string>& filenames);

// name of the default parameterization
Pulsar::Calibrator::Type model_name = Pulsar::Calibrator::Britton;

// class name of the calibrator solution archives to be produced
string archive_class = "PSRFITS";

// plot the solution before calibrating with it
bool display = true;

// verbosity flags
bool verbose = false;

//! The maximum number of bins to use
unsigned maxbins = 16;

//! Flag raised when the above value is set using -n
bool maxbins_set = false;

int main (int argc, char *argv[]) try {

  Error::verbose = false;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of file containing list of calibrator Archive filenames
  char* calfile = NULL;

  // name of file containing a Calibration Database
  char* dbfile = NULL;

  // name of file containing the calibrated standard
  char* stdfile = NULL;

  // name of file from which phase bins will be chosen
  char* binfile = NULL;

  // number of hours over which CALs will be found from Database
  float hours = 24.0;


  //! The pulse phase window to use
  float phmin, phmax;

  phmin = phmax = 0.0;

  //! The channel to solve (-f specified)
  int only_ichan = -1;

  //! The phase bins to add to the model
  vector<unsigned> phase_bins;

  bool measure_cal_V = true;
  bool measure_cal_Q = true;

  bool normalize_by_invariant = true;
  bool must_have_cals = true;
  bool publication_plots = false;

  int gotc = 0;
  const char* args = "a:b:c:C:d:Df:hM:m:n:OPp:qsS:t:uvV:";
  while ((gotc = getopt(argc, argv, args)) != -1) {
    switch (gotc) {

    case 'a':
      archive_class = optarg;
      break;

    case 'b': {
      unsigned bin = atoi (optarg);
      cerr << "pcm: adding phase bin " << bin << endl;
      phase_bins.push_back (bin);
      break;
    }

    case 'C':
      calfile = optarg;
      break;

    case 'c':
      binfile = optarg;
      break;

    case 'd':
      dbfile = optarg;
      break;

    case 'D':
      display = false;
      break;

    case 'f':
      only_ichan = atoi (optarg);
      cerr << "pcm: solving only channel " << only_ichan << endl;
      break;

    case 'm':
      if (optarg == Britton)
	model_name = Pulsar::Calibrator::Britton;
      else if (optarg == Hamaker)
	model_name = Pulsar::Calibrator::Hamaker;
      else {
	cerr << "pcm: unrecognized model name '" << optarg << "'" << endl;
	return -1;
      }
      break;
      
    case 'M':
      metafile = optarg;
      break;

    case 'n':
      maxbins = atoi (optarg);
      cerr << "pcm: using a maximum of " << maxbins << " bins or harmonics" 
	   << endl;
      maxbins_set = true;
      break;

    case 'O':
      Calibration::Model::signal_changed = false;
      break;

    case 'P':
      publication_plots = true;
      break;

    case 'p':
      if (sscanf (optarg, "%f,%f", &phmin, &phmax) != 2) {
	cerr << "pcm: error parsing " << optarg << " as phase window" << endl;
	return -1;
      }
      cerr << "pcm: selecting input states from " << phmin << " to " << phmax
	   << endl;
      break;

    case 's':
      normalize_by_invariant = false;
      break;

    case 'S':
      stdfile = optarg;
      break;

    case 't':  {

      unsigned nthreads = atoi (optarg);
      if (nthreads == 0)  {
        cerr << "pcm: invalid number of threads = " << nthreads << endl;
        return -1;
      }

      Calibration::ReceptionModel::set_nsolve (nthreads);

      cerr << "pcm: solving using " << nthreads << " threads" << endl;
      break;

    }

    case 'q':
      measure_cal_Q = false;
      break;

    case 'v':
      measure_cal_V = false;
      break;


    case 'h':
      usage ();
      return 0;

    case 'V': {

      int level = atoi (optarg);
      verbose = true;

      if (level > 3)
        Calibration::Model::very_verbose = true;

      if (level > 2) 
        Calibration::Model::verbose = true;

      if (level > 1)
        Pulsar::Calibrator::verbose = true;

      Pulsar::Archive::set_verbosity (level);

      break;

    }

    default:
      cout << "Unrecognised option" << endl;
    }
  }

  if (!stdfile && phmin == phmax && !binfile) {
    cerr << "pcm: In mode A, at least one of the following options"
      " must be specified:\n"
      " -p min,max  Choose constraints from the specified pulse phase range \n"
      " -c archive  Choose optimal constraints from the specified archive \n"
	 << endl;
    return -1;
  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);
  
  if (filenames.empty()) {
    cerr << "pcm: no archives were specified" << endl;
    return -1;
  } 

  // assumes that sorting by filename also sorts by epoch
  sort (filenames.begin(), filenames.end());

  if (stdfile)
    return mode_B (stdfile, filenames);

  // the reception calibration class
  Pulsar::ReceptionCalibrator model (model_name);

  if (measure_cal_V)
    cerr << "pcm: assuming that System + Hydra A Stokes V = 0" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes V = 0" << endl;

  model.measure_cal_V = measure_cal_V;

  if (measure_cal_Q)
    cerr << "pcm: allowing CAL Stokes Q to vary" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes Q = 0" << endl;

  model.measure_cal_Q = measure_cal_Q;

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant interval" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model.normalize_by_invariant = normalize_by_invariant;

  // add the specified phase bins
  for (unsigned ibin=0; ibin<phase_bins.size(); ibin++)
    model.add_state (phase_bins[ibin]);

  // add the calibrators (to be loaded on first call to add_observation
  vector<string> cal_filenames;

  if (calfile)
    stringfload (&cal_filenames, calfile);

  Reference::To<Pulsar::Archive> archive;

  if (dbfile) {

    archive = Pulsar::Archive::load(filenames.back());
    MJD end = archive->end_time();

    archive = Pulsar::Archive::load(filenames.front());
    MJD start = archive->start_time();

    MJD mid = 0.5 * (end + start);

    cerr << "pcm: constructing Calibration::Database from\n" 
            "     " << dbfile << endl;

    Pulsar::Database dbase (dbfile);

    char buffer[256];

    cerr << "pcm: searching for calibrator observations within " << hours
	 << " hours of midtime" << endl;
    cerr << "pcm: midtime = "
         << mid.datestr (buffer, 256, "%Y-%m-%d-%H:%M:00") << endl;

    Pulsar::Database::Criterion criterion;
    criterion = Pulsar::Database::get_default_PolnCal_criterion ();
    criterion.entry = Pulsar::Database::Entry (*archive);
    criterion.entry.time = mid;
    criterion.minutes_apart = 0.5 * hours * 60.0;

    vector<Pulsar::Database::Entry> oncals;

    criterion.entry.obsType = Signal::PolnCal;
    oncals = dbase.all_matching (criterion);

    if (oncals.size() == 0)  {
      cerr << "pcm: no PolnCal observations found" << endl;
      if (must_have_cals && !calfile)  {
        cerr << "pcm: giving up" << endl;
        return -1;
      }
    }

    for (unsigned i = 0; i < oncals.size(); i++) {
      string filename = dbase.get_filename( oncals[i] );
      cerr << "pcm: adding " << oncals[i].filename << endl;
      cal_filenames.push_back (filename);
    }

    criterion.entry.obsType = Signal::FluxCalOn;
    oncals = dbase.all_matching (criterion);

    if (oncals.size() == 0)
      cerr << "pcm: no FluxCalOn observations found" << endl;

    for (unsigned i = 0; i < oncals.size(); i++) {
      string filename = dbase.get_filename( oncals[i] );
      cerr << "pcm: adding " << oncals[i].filename << endl;
      cal_filenames.push_back (filename);
    }

  }

  cerr << "pcm: set calibrators" << endl;
  model.set_calibrators (cal_filenames);
  

  Reference::To<Pulsar::Archive> autobin;

  if (binfile) try {
    autobin = Pulsar::Archive::load (binfile);
    autobin->dedisperse ();
    autobin->fscrunch ();
    autobin->tscrunch ();
    autobin->convert_state (Signal::Stokes);
    autobin->remove_baseline ();
  }
  catch (Error& error) {
    cerr << "pcm: could not load constraint archive " << binfile << endl
	 << error.warning() << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> total;
  
  cerr << "pcm: loading archives" << endl;
  
  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {

      if (!archive) {

	if (verbose)
	  cerr << "pcm: loading " << filenames[i] << endl;
	
	archive = Pulsar::Archive::load(filenames[i]);

        if (!archive) {
          cerr << "pcm: error loading " << filenames[i] << endl;
          continue;
        }

	cout << "pcm: loaded archive: " << filenames[i] << endl;
      
      }

      if (archive->get_type() == Signal::Pulsar)  {
	
	if (verbose)
	  cerr << "pcm: dedispersing and removing baseline from pulsar data"
               << endl;
	
        archive->dedisperse ();
	archive->convert_state (Signal::Stokes);
        archive->remove_baseline ();
      }

      if (model.get_nstate_pulsar() == 0) {

	if (autobin) {

#if 0
	  string reason;
	  if ( ! archive->mixable (autobin, reason) ) {
	    cerr << "pcm: cannot choose constraints from " << binfile
		 << endl << reason << endl;
	    return -1;
	  }
#endif

	  auto_select (model, autobin, maxbins);

	}

	else
	  range_select (model, archive, phmin, phmax, maxbins);

        cerr << "pcm: " << model.get_nstate_pulsar() << " states" << endl;
      }


      model.add_observation( archive );

      if (archive->get_type() == Signal::Pulsar && only_ichan < 0)  {

	if (verbose)
	  cerr << "pcm: calibrate with current best guess" << endl;

	model.precalibrate (archive);

	if (verbose)
	  cerr << "pcm: fscrunch, correct, and add to total" << endl;

        archive->fscrunch ();
        archive->correct_instrument ();

	if (!total)
	  total = archive;
	else {
	  total->append (archive);
	  total->tscrunch ();
	}
      }

      archive = 0;

    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }

  if (total)  {
    cerr << "pcm: writing total uncalibrated pulsar archive" << endl;
    total->unload ("first.ar");
  }

  Pulsar::ReceptionCalibratorPlotter plotter (&model);
  plotter.use_colour = !publication_plots;

  if (display && only_ichan < 0) {


    cpgbeg (0, "guess.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (.1,.9, .1,.9);

    cerr << "pcm: plotting initial guess of receiver" << endl;
    plotter.plot (&model);

    cpgpage();

    cerr << "pcm: plotting uncalibrated CAL" << endl;
    plotter.plotcal();

    cpgend();

    cpgbeg (0, "uncalibrated.ps/CPS", 0, 0);
    cpgask(1);
    cpgslw(2);
    cpgsvp (.1,.9, .1,.9);

    cerr << "pcm: plotting uncalibrated pulsar total stokes" << endl;
    Pulsar::Plotter profile;
    profile.spherical (total);

    cpgend();

    cpgbeg (0, "guess_source.ps/CPS", 0, 0);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting guess pulsar states" << endl;
    plot_pulsar (plotter, model);

    cpgend ();

    cerr << "pcm: plotting pulsar constraints" << endl;
    plot_constraints (plotter, model.get_nchan(),
		      model.get_nstate_pulsar(), 12, 
		      model.get_nstate()-model.get_nstate_pulsar());


  }

  total = 0;

  cerr << "pcm: solving model" << endl;

  try {
    model.solve (only_ichan);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> solution = model.get_solution (archive_class);

  cerr << "pcm: unloading solution to " << solution->get_filename() << endl;
  solution->unload( "pcm.fits" );

  if (display) {

    cpgbeg (0, "result.ps/CPS", 0, 0);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting model receiver" << endl;
    plotter.plot (&model);

    cpgpage();

    cerr << "pcm: plotting calibrated CAL" << endl;
    plotter.plotcal();

    cpgend ();

    cpgbeg (0, "source.ps/CPS", 0, 0);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting model pulsar states" << endl;
    plot_pulsar (plotter, model);

    cpgend ();

    cerr << "pcm: plotting pulsar constraints with model" << endl;
    plot_constraints (plotter, model.get_nchan(),
		      model.get_nstate_pulsar(), 12,
		      model.get_nstate()-model.get_nstate_pulsar());

  }

  for (unsigned ical=0; ical < cal_filenames.size(); ical++)
    dirglob (&filenames, cal_filenames[ical]);
  
  cerr << "pcm: calibrating archives (PSR and CAL)" << endl;

  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {

      if (verbose)
	cerr << "pcm: loading " << filenames[i] << endl;

      archive = Pulsar::Archive::load(filenames[i]);

      cout << "pcm: loaded archive: " << filenames[i] << endl;
      
      model.precalibrate( archive );

      if (archive->get_type() == Signal::Pulsar)
	archive->correct_instrument ();

      int index = filenames[i].find_first_of(".", 0);
      string newname = filenames[i].substr(0, index);
      newname += ".calib";

      if (verbose)
        cerr << "pcm: calibrated Archive name '" << newname << "'" << endl;

      archive->unload (newname);
      
      cout << "New file " << newname << " unloaded" << endl;

      if (archive->get_type() == Signal::Pulsar)  {

        if (verbose)
          cerr << "pcm: fscrunch and add to calibrated total" << endl;

        archive->fscrunch ();
        archive->correct_instrument ();

        if (!total)
          total = archive;
        else {
          total->append (archive);
          total->tscrunch ();
        }
      }

    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }

  if (total)  {
    cerr << "pcm: writing total integrated pulsar archive" << endl;
    total->unload ("total.ar");
  }

  if (display) {

    cpgbeg (0, "calibrated.ps/CPS", 0, 0);
    cpgask(1);
    cpgslw(2);
    cpgsvp (.1,.9, .1,.9);

    cerr << "pcm: plotting calibrated pulsar total stokes" << endl;
    Pulsar::Plotter profile;
    profile.spherical (total);

    cpgend ();
  }

  return 0;
}
catch (Error& error) {
  cerr << "pcm: error" << error << endl;
  return -1;
}

int mode_B (const char* standard_filename,
	    const vector<string>& filenames)
{
  // the reception calibration class
  Pulsar::PulsarCalibrator model (model_name);

  if (maxbins_set)
    model.set_maximum_harmonic (maxbins);

  model.set_return_mean_solution (false);

  Reference::To<Pulsar::Archive> standard;

  standard = Pulsar::Archive::load (standard_filename);
  standard->convert_state (Signal::Stokes);

  RealTimer clock;

  clock.start();

  model.set_standard (standard);

  clock.stop();
  cerr << "pcm: set_standard completed in " << clock << endl;

  if (verbose)
    cerr << "pcm: loading " << filenames.size() << " files" << endl;

  Reference::To<Pulsar::Archive> archive;
  Reference::To<Pulsar::Archive> solution;

  bool add_to_standard = false;

  for (unsigned i = 0; i < filenames.size(); i++) try {

    if (verbose)
      cerr << "pcm: loading " << filenames[i] << endl;
    
    archive = Pulsar::Archive::load(filenames[i]);
    archive->convert_state (Signal::Stokes);

    cout << "pcm: loaded archive: " << filenames[i] << endl;

    model.add_observation( archive );

    string filename = filenames[i] + ".fits";
    cerr << "pcm: unloading solution to " << filename << endl;

    solution = model.get_solution (archive_class);
    solution->unload( filename );

    if (add_to_standard) {

      model.calibrate( archive );

      archive->correct_instrument ();

      archive->convert_state (Signal::Stokes);
      standard->append (archive);

      standard->tscrunch ();

    }

  }
  catch (Error& error) {
    cerr << error << endl;
  }

  model.set_return_mean_solution (true);
  model.update_solution ();

  solution = model.get_solution (archive_class);

  cerr << "pcm: unloading solution to pcm.fits" << endl;
  solution->unload( "pcm.fits" );

  if (add_to_standard) {
    cerr << "pcm: unloading updated standard to pcm.std" << endl;
    standard->unload( "pcm.std" );
  }

  return 0;
}
