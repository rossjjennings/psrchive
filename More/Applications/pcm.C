//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pcm.C,v $
   $Revision: 1.84 $
   $Date: 2008/06/05 20:34:33 $
   $Author: straten $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/psrchive.h"
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/Database.h"

#include "Pulsar/ReceptionModelSolveMEAL.h"
#if HAVE_GSL
#include "Pulsar/ReceptionModelSolveGSL.h"
#endif

#include "MEAL/Steps.h"

#include "Pulsar/Interpreter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ReflectStokes.h"

#include "RealTimer.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

#if HAVE_PGPLOT
#include "Pulsar/SystemCalibratorPlotter.h"
#include "Pulsar/StokesSpherical.h"
#include <cpgplot.h>
#endif

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <errno.h>

using namespace std;
using namespace Pulsar;

static string Britton = "Britton";
static string Hamaker = "Hamaker";

void usage ()
{
  cout << "pcm - polarimetric calibration model \n"
    "Usage: pcm [options] [filenames] \n"
    "\n"
    "  -h         this help page \n"
    "  -V level   set verbosity level [0->4] \n"
    "  -A archive set the output archive class name \n"
    "  -m model   model: Britton [default] or Hamaker \n"
    "  -l solver  solver: MEAL [default] of GSL \n"
    "\n"
    "  -C meta    filename with list of calibrator files \n"
    "  -d dbase   filename of Calibration Database \n"
    "  -M meta    filename with list of pulsar files \n"
    "  -j job     preprocessing job \n"
    "  -J jobs    multiple preprocessing jobs in 'jobs' file \n"
    "\n"
    "MODE A: Fit multiple observations of unknown source \n"
    "\n"
    "  -t nproc   solve using nproc threads \n"
    "\n"
    "  -u PAR     model PAR with a step at each CAL \n"
    "  -o PAR:N   model PAR as N degree polyomial \n"
    "             where PAR is one of \n"
    "               g = absolute gain \n"
    "               b = differential gain \n"
    "               r = differential phase \n"
    "               a = all of the above \n"
    "\n"
    "  -b bin     add phase bin to constraints \n"
    "  -n nbin    set the number of phase bins to choose as input states \n"
    "  -p pA,pB   set the phase window from which to choose input states \n"
    "  -c archive choose best input states from input archive \n"
    "\n"
    "  -a align   set the threshold for testing input data phase alignment \n"
    "  -g         unique absolute gain for each pulsar observation [DEVEL]\n"
    "  -r         enforce physically realizable Stokes parameters [DEVEL]\n"
    "  -s         normalize Stokes parameters by invariant interval \n"
    "\n"
    "  -q         assume that CAL Stokes Q = 0 (linear feeds only)\n"
    "  -v         assume that CAL Stokes V = 0 (linear feeds only)\n"
    "  -L hours   maximum time between middle of experiment and calibrators\n"
    "\n"
    "MODE B: Fit observations to a known source (matrix template matching) \n"
    "\n"
    "  -S fname   filename of calibrated standard \n"
    "  -H         allow software to choose the number of harmonics \n"
    "  -n nbin    set the number of harmonics to use as input states \n"
    "  -T toa.tim filename to which arrival time estimates will be written \n"
    "  -1         solve independently for each observation \n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/pcm for more details\n"
       << endl;
}

// Construct a calibrator model for mode A
SystemCalibrator* time_variation_based (const char* binname, unsigned nbin);

// Construct a calibrator model for mode B
SystemCalibrator* matrix_template_matching_based (const char* stdname);

// defined in More/Polarimetry/choose.C
void choose (vector<unsigned>& bins, Pulsar::Archive* archive);

void auto_select (Pulsar::ReceptionCalibrator& model,
		  Pulsar::Archive* archive,
		  unsigned maxbins)
{
  cerr << "pcm: choosing up to " << maxbins << " pulse phase bins" << endl;
  vector<unsigned> bins (maxbins);

  choose (bins, archive);

  sort (bins.begin(), bins.end());

  for (unsigned ibin=0; ibin < bins.size(); ibin++) {
    // cerr << "pcm: adding phase bin " << bins[ibin] << endl;
    model.add_state (bins[ibin]);
  }

  model.set_standard_data( archive );

#if HAVE_PGPLOT
  cpgbeg (0, "chosen.ps/CPS", 0, 0);
  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  cerr << "pcm: plotting chosen phase bins" << endl;
  Pulsar::StokesSpherical plot;
  plot.plot (archive);

  cpgswin (0,1,0,1);
  cpgsls (2);
  for (unsigned ibin=0; ibin < bins.size(); ibin++) {
    float phase = float(bins[ibin])/float(archive->get_nbin());
    cpgmove (phase, 0);
    cpgdraw (phase, 1);
  }

  cpgend();
#endif
}


void range_select (Pulsar::ReceptionCalibrator& model,
		   float phmin, float phmax,
		   unsigned nbin, unsigned maxbins)
{
  if (phmin > phmax)
    phmax += 1.0;

  float increment = (phmax - phmin)/maxbins;

  unsigned last_bin = unsigned (phmax * nbin);

  for (float bin = phmin; bin<=phmax; bin += increment) {

    unsigned ibin = unsigned (bin * nbin) % nbin;

    if (ibin != last_bin)  {
      cerr << "pcm: adding phase bin " << ibin << endl;
      model.add_state (ibin%nbin);
      last_bin = ibin;
    }
  }
}

#if HAVE_PGPLOT

void plot_pulsar (Pulsar::SystemCalibratorPlotter& plotter,
		  Pulsar::SystemCalibrator& model)
{

  unsigned panels = plotter.npanel;
  plotter.npanel = 4;

  unsigned nstate = model.get_nstate_pulsar();
  double centre_frequency = model.get_Archive()->get_centre_frequency();
  double bandwidth = model.get_Archive()->get_bandwidth();

  Reference::To<Pulsar::Calibrator::Info> info;

  for (unsigned istate=0; istate<nstate; istate++)
  {
    info = model.new_info_pulsar (istate);
    cpgpage();
    plotter.plot (info, model.get_nchan(), centre_frequency, bandwidth);
  }

  plotter.npanel = panels;

}

void plot_constraints (Pulsar::SystemCalibratorPlotter& plotter,
		       unsigned nchan, unsigned nstate, unsigned nplot,
		       unsigned start_state, unsigned only_chan=0)
{
  if (nstate == 0)
    return;

  unsigned incr = nchan/nplot;
  if (!incr)
    incr = 1;

  unsigned ichan = 1;

  if (nplot == 1)
    ichan = only_chan;

  if (nstate == 1)
    cpgbeg (0, "states.ps/CPS", 0, 0);

  for (; ichan < nchan; ichan+=incr)
  {
    // don't try to plot if the equation for this channel has no data
    while (plotter.get_calibrator()->get_ndata (ichan) == 0)
    {
      ichan ++;
      if (ichan >= nchan)
      {
	cpgend();
	return;
      }
    }

    if (nstate > 1)
    {
      char filename [256];
      sprintf (filename, "channel_%d.ps/CPS", ichan);
      cpgbeg (0, filename, 0, 0);
    }

    if (plotter.use_colour)
      cpgsvp (.15,.9, .15,.9);
    else
      cpgsvp (.25,.75,.15,.95);

    plotter.plot_cal_constraints (ichan);

    // cerr << "pcm: nstate=" << nstate << endl;
    for (unsigned istate=0; istate<nstate; istate++)
    {
      cpgpage();

      unsigned plot_state = istate+start_state;

      // cerr << "ichan=" << ichan << " istate=" << plot_state << endl;
      plotter.plot_psr_constraints (ichan, plot_state);
    }

    if (nstate > 1)
      cpgend ();
    else if (ichan+incr < nchan)
      cpgpage ();

  }

  if (nstate == 1)
    cpgend ();

}

#endif // HAVE_PGPLOT

// name of the default parameterization
Pulsar::Calibrator::Type model_type = Pulsar::Calibrator::Britton;

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

//! The pulse phase window to use
float phmin = 0, phmax = 0;

//! The phase bins to add to the model
vector<unsigned> phase_bins;

//! Flag raised when software may choose the maximum harmonic
bool choose_maximum_harmonic = false;

//! Mode B: Solve the measurement equation for each observation
bool solve_each = false;

// significance of phase shift required to fail test
float alignment_threshold = 4.0; // sigma

//! The Stokes parameters to be inverted
Pulsar::ReflectStokes reflections;

// total instensity profile of first archive, used to check for phase jumps
Reference::To<Pulsar::Profile> phase_std;

// Name of file to which arrival time estimates will be written
char* tim_file = 0;

// names of the calibrator files
vector<string> calibrator_filenames;

bool measure_cal_V = true;
bool measure_cal_Q = true;

bool normalize_by_invariant = false;
bool independent_gains = false;

bool physical_coherency = false;

int actual_main (int argc, char *argv[]);

int main (int argc, char *argv[])
{
#ifdef _DEBUG
  size_t in = Reference::Able::get_instance_count();
#endif

  int ret = actual_main (argc, argv);

#ifdef _DEBUG
  size_t out = Reference::Able::get_instance_count();
  cerr << "Leaked: " << out - in << endl;
#endif

  return ret;
}

Reference::To< MEAL::Univariate<MEAL::Scalar> > gain_variation;
Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_gain_variation;
Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_phase_variation;

bool get_time_variation ()
{
  return gain_variation || diff_gain_variation || diff_phase_variation;
}

void set_time_variation (char code, MEAL::Univariate<MEAL::Scalar>* function)
{
  switch (code) {
  case 'g':
    cerr << "gain" << endl;
    gain_variation = function;
    return;
  case 'b':
    cerr << "differential gain" << endl;
    diff_gain_variation = function;
    return;
  case 'r':
    cerr << "differential phase" << endl;
    diff_phase_variation = function;
    return;
  case 'a':
    cerr << "all backend parameters" << endl;
    gain_variation = function;
    diff_gain_variation = function;
    diff_phase_variation = function;
    return;
  }
  throw Error (InvalidParam, "set_time_variation",
	       "unrecognized PAR code = %c", code);
}

Calibration::ReceptionModel::Solver* new_solver (const string& name)
{
  if (name == "MEAL")
    return new Calibration::SolveMEAL;

#if HAVE_GSL
  if (name == "GSL")
    return new Calibration::SolveGSL;
#endif

  throw Error (InvalidParam, "pcm", "no solver named " + name);
}

int actual_main (int argc, char *argv[]) try
{
  // Number of threads used to solve equations
  unsigned nthread = 0;

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

  // name of least squares minimization algorithm
  char* least_squares = NULL;

  // number of hours over which CALs will be found from Database
  float hours = 12.0;

  // preprocessing jobs
  vector<string> jobs;

  bool must_have_cals = true;
  bool publication_plots = false;

  int gotc = 0;
  const char* args = "1:A:a:b:c:C:d:DgHhIj:J:L:l:M:m:N:n:o:Pp:qrsS:t:T:u:vV:";
  while ((gotc = getopt(argc, argv, args)) != -1)
  {
    switch (gotc)
    {
    case '1':
      solve_each = true;
      break;

    case 'A':
      archive_class = optarg;
      break;

    case 'a':
      alignment_threshold = atof (optarg);
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

    case 'g':
      independent_gains = true;
      break;
      
    case 'H':
      choose_maximum_harmonic = true;
      break;

    case 'j':
      separate (optarg, jobs, ",");
      break;

    case 'J':
      loadlines (optarg, jobs);
      break;

    case 'L':
      hours = atof (optarg);
      break;

    case 'l':
      least_squares = optarg;
      break;

    case 'm':
      if (optarg == Britton)
	model_type = Pulsar::Calibrator::Britton;
      else if (optarg == Hamaker)
	model_type = Pulsar::Calibrator::Hamaker;
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

    case 'N':
      reflections.add_reflection (optarg[0]);
      break;

    case 'o': {
      char code;
      unsigned order;
      if( sscanf (optarg, "%c:%u", &code, &order) != 2 ) {
	cerr << "pcm: error parsing '" << optarg << "' as PAR:N" << endl;
	return -1;
      }
      cerr << "pcm: using a polynomial of degree " << order << " to model ";
      set_time_variation( code, new MEAL::Polynomial (order+1) );
      break;
    }

    case 'P':
      publication_plots = true;
      break;

    case 'p':
      if (sscanf (optarg, "%f,%f", &phmin, &phmax) != 2) {
	cerr << "pcm: error parsing " << optarg << " as phase range" << endl;
	return -1;
      }
      cerr << "pcm: selecting input states from " << phmin << " to " << phmax
	   << endl;
      break;

    case 'r':
      physical_coherency = true;
      break;

    case 's':
      normalize_by_invariant = true;
      break;

    case 'S':
      alignment_threshold = 0.0;
      stdfile = optarg;
      break;

    case 't':

      nthread = atoi (optarg);
      if (nthread == 0)  {
        cerr << "pcm: invalid number of threads = " << nthread << endl;
        return -1;
      }

      cerr << "pcm: solving using " << nthread << " threads" << endl;
      break;

    case 'T':
      tim_file = optarg;
      break;

    case 'u':
      cerr << "pcm: using a multiple-step function to model ";
      set_time_variation( optarg[0], new MEAL::Steps );
      break;

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
        Calibration::ReceptionModel::very_verbose = true;

      if (level > 2) 
        Calibration::ReceptionModel::verbose = true;

      Calibration::StandardModel::verbose = true;
      Pulsar::Calibrator::verbose = level;
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
  
  if (filenames.empty())
  {
    cerr << "pcm: no archives were specified" << endl;
    return -1;
  } 

  // assumes that sorting by filename also sorts by epoch
  sort (filenames.begin(), filenames.end());

  if (calfile)
    stringfload (&calibrator_filenames, calfile);

  Reference::To<Pulsar::Archive> archive;

  if (dbfile)
  {
    archive = Pulsar::Archive::load(filenames.back());
    MJD end = archive->end_time();

    archive = Pulsar::Archive::load(filenames.front());
    MJD start = archive->start_time();

    MJD mid = 0.5 * (end + start);

    cerr << "pcm: constructing Calibration::Database from\n" 
            "\t" << dbfile << endl;

    Pulsar::Database database (dbfile);

    cerr << "pcm: database constructed with " << database.size() 
         << " entries" << endl;

    char buffer[256];

    cerr << "pcm: searching for calibrator observations within " << hours
	 << " hours of midtime" << endl;
    cerr << "pcm: midtime = "
         << mid.datestr (buffer, 256, "%Y-%m-%d-%H:%M:00") << endl;

    Pulsar::Database::Criterion criterion;
    criterion = database.criterion (archive, Signal::PolnCal);
    criterion.entry.time = mid;
    criterion.minutes_apart = hours * 60.0;

    vector<Pulsar::Database::Entry> oncals;
    database.all_matching (criterion, oncals);

    unsigned poln_cals = oncals.size();

    if (poln_cals == 0)  {
      cerr << "pcm: no PolnCal observations found" << endl;
      if (must_have_cals && !calfile)  {
        cerr << "pcm: cannot continue" << endl;
        return -1;
      }
    }

    criterion.entry.obsType = Signal::FluxCalOn;
    criterion.check_coordinates = false;
    database.all_matching (criterion, oncals);

    if (oncals.size() == poln_cals)
      cerr << "pcm: no FluxCalOn observations found" << endl;

    for (unsigned i = 0; i < oncals.size(); i++) {
      string filename = database.get_filename( oncals[i] );
      cerr << "pcm: adding " << oncals[i].filename << endl;
      calibrator_filenames.push_back (filename);
    }

  }

  Reference::To<Pulsar::Interpreter> preprocessor = standard_shell();

  Reference::To<Pulsar::SystemCalibrator> model;

  Reference::To<Pulsar::Archive> total;

  cerr << "pcm: loading archives" << endl;
  
  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    if (!archive)
    {
      if (verbose)
	cerr << "pcm: loading " << filenames[i] << endl;
	
      archive = Pulsar::Archive::load(filenames[i]);
      reflections.transform (archive);

      cout << "pcm: loaded archive: " << filenames[i] << endl;
    }

    if (jobs.size())
    {
      if (verbose)
        cerr << "pcm: preprocessing " << archive->get_filename() << endl;
      preprocessor->set(archive);
      preprocessor->script(jobs);
    }

    if (archive->get_type() == Signal::Pulsar)
    {
      if (verbose)
	cerr << "pcm: dedispersing and removing baseline from pulsar data"
	     << endl;
      
      archive->convert_state (Signal::Stokes);
      archive->remove_baseline ();
      archive->dedisperse ();
      archive->centre (0.0);
    }

    if (!model) try
    {
      if (verbose)
	cerr << "pcm: creating mdodel" << endl;

      if (stdfile)
	model = matrix_template_matching_based (stdfile);
      else
	model = time_variation_based (binfile, archive->get_nbin());

      model->set_nthread (nthread);

      if (gain_variation)
	model->set_gain( gain_variation );

      if (diff_gain_variation)
	model->set_diff_gain( diff_gain_variation );

      if (diff_phase_variation)
	model->set_diff_phase( diff_phase_variation );

      if (least_squares)
	model->set_solver( new_solver(least_squares) );

    }
    catch (Error& error)
    {
      cerr << "pcm: ERROR while creating model\n" << error << endl;
      return -1;
    }

    /*
      test for phase shift only if phase_std is not from current archive.
      this test will fail if binfile is a symbollic link.
    */
    if (phase_std && (binfile==NULL || archive->get_filename() != binfile)) try
    {
      if (verbose)
	cerr << "pcm: creating checking phase" << endl;

      Reference::To<Pulsar::Archive> temp = archive->total();
      Estimate<double> shift = temp->get_Profile(0,0,0)->shift (*phase_std);

      double abs_shift = fabs( shift.get_value() );

      /* if the shift is greater than 1 phase bin and significantly
	 more than the error, then there may be a problem */

      if( abs_shift > 1.0 / phase_std->get_nbin() &&
	  abs_shift > alignment_threshold * shift.get_error() )
      {
	cerr << endl <<
	  "pcm: ERROR apparent phase shift between input archives\n"
	  "\tshift = " << shift.get_value() << " +/- " << shift.get_error () <<
	  "  =  " << int(shift.get_value() * phase_std->get_nbin()) <<
	  " phase bins" << endl << endl;

	archive = 0;
	continue;
      }
    }
    catch (Error& error)
    {
      cerr << "pcm: ERROR while testing phase shift\n" << error << endl;
      return -1;
    }

    if (alignment_threshold && !phase_std)
    { 
      cerr << "pcm: creating phase reference" << endl;

      // store an fscrunched and tscrunched clone for phase reference
      Reference::To<Archive> temp = archive->total();
      phase_std = temp->get_Profile (0,0,0);	
    }


    model->add_observation( archive );

    if (archive->get_type() == Signal::Pulsar)
    {    
      if (verbose)
	cerr << "pcm: calibrate with current best guess" << endl;
      
      model->precalibrate (archive);
      
      if (verbose)
	cerr << "pcm: correct and add to total" << endl;
      
      Pulsar::FrontendCorrection correct;
      correct.calibrate(archive);

      if (!total)
	total = archive;
      else
      {
	total->append (archive);
	total->tscrunch ();
      }
    }
    archive = 0;
  }
  catch (Error& error)
  {
    cerr << "pcm: error while handling " << filenames[i] << endl;
    cerr << error << endl;
    archive = 0;
  }

  if (total)
  {
    cerr << "pcm: writing total uncalibrated pulsar archive" << endl;
    total->unload ("first.ar");
  }

#if HAVE_PGPLOT

  Pulsar::SystemCalibratorPlotter plotter (model);
  plotter.use_colour = !publication_plots;

  if (display)
  {
    cpgbeg (0, "guess.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (.1,.9, .1,.9);

    cerr << "pcm: plotting initial guess of receiver" << endl;
    plotter.plot (model);

    cpgpage();

    cerr << "pcm: plotting initial guess of CAL" << endl;
    plotter.plotcal();

    cpgend();

    if (total)
    {
      cerr << "pcm: plotting uncalibrated total PSR" << endl;

      cpgbeg (0, "uncalibrated.ps/CPS", 0, 0);
      cpgask(1);
      cpgslw(2);
      cpgsvp (.1,.9, .1,.9);
      
      total->fscrunch();
      
      cerr << "pcm: plotting uncalibrated pulsar total stokes" << endl;
      Pulsar::StokesSpherical plot;
      plot.plot (total);
      
      cpgend();
    }

    if (model->get_nstate_pulsar())
    {
      cpgbeg (0, "guess_source.ps/CPS", 0, 0);
      cpgsvp (0.1,.9, 0.1,.9);

      cerr << "pcm: plotting guess pulsar states" << endl;
      plot_pulsar (plotter, *model);

      cpgend ();

      cerr << "pcm: plotting pulsar constraints" << endl;
      plot_constraints (plotter, model->get_nchan(),
			model->get_nstate_pulsar(), 12, 
			model->get_nstate()-model->get_nstate_pulsar());
    }

  }

#endif // HAVE_PGPLOT

  total = 0;

  try
  {
    cerr << "pcm: solving model" << endl;
    model->solve ();
  }
  catch (Error& error)
  {
    cerr << error << endl;
    return -1;
  }

  cerr << "pcm: creating solution" << endl;
  Reference::To<Archive> solution = model->new_solution (archive_class);

  cerr << "pcm: unloading solution to pcm.fits" << endl;
  solution->unload( "pcm.fits" );

#if HAVE_PGPLOT

  if (display)
  {

    cpgbeg (0, "result.ps/CPS", 0, 0);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting best-fit receiver" << endl;
    plotter.plot (model);

    cpgpage();

    cerr << "pcm: plotting best-fit CAL" << endl;
    plotter.plotcal();

    cpgend ();

    if (get_time_variation())
    {
      cpgbeg (0, "variations.ps/PS", 0, 0);
      cpgsvp (0.1,.9, 0.1,.9);

      unsigned panels = plotter.npanel;
      plotter.npanel = 4;

      cerr << "pcm: plotting time variation functions" << endl;
      plotter.plot_time_variations ();

      plotter.npanel = panels;
      cpgend ();
    }

    if (model->get_nstate_pulsar())
    {
      cpgbeg (0, "source.ps/CPS", 0, 0);
      cpgsvp (0.1,.9, 0.1,.9);

      cerr << "pcm: plotting model pulsar states" << endl;
      plot_pulsar (plotter, *model);

      cpgend ();
      
      plotter.set_plot_residual (!normalize_by_invariant);
      
      cerr << "pcm: plotting pulsar constraints with model" << endl;
      plot_constraints (plotter, model->get_nchan(),
			model->get_nstate_pulsar(), 12,
			model->get_nstate()-model->get_nstate_pulsar());
    }

  }

#endif // HAVE_PGPLOT

  for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
    dirglob (&filenames, calibrator_filenames[ical]);
  
  cerr << "pcm: calibrating archives (PSR and CAL)" << endl;

  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {

      if (verbose)
	cerr << "pcm: loading " << filenames[i] << endl;

      archive = Pulsar::Archive::load(filenames[i]);
      reflections.transform (archive);

      cout << "pcm: loaded archive: " << filenames[i] << endl;
      
      model->precalibrate( archive );

      if (archive->get_type() == Signal::Pulsar)
      {
	Pulsar::FrontendCorrection correct;
	correct.calibrate(archive);
      }

      string newname = replace_extension (filenames[i], ".calib");

      if (verbose)
        cerr << "pcm: calibrated Archive name '" << newname << "'" << endl;

      archive->unload (newname);
      
      cout << "New file " << newname << " unloaded" << endl;

      if (archive->get_type() == Signal::Pulsar)  {

        if (verbose)
          cerr << "pcm: correct and add to calibrated total" << endl;

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

#if HAVE_PGPLOT

  if (display) {

    cpgbeg (0, "calibrated.ps/CPS", 0, 0);
    cpgask(1);
    cpgslw(2);
    cpgsvp (.1,.9, .1,.9);

    total->fscrunch();
    total->remove_baseline();

    cerr << "pcm: plotting calibrated pulsar total stokes" << endl;
    Pulsar::StokesSpherical plot;
    plot.plot (total);

    cpgend ();
  }

#endif // HAVE_PGPLOT

  cerr << "pcm: finished" << endl;

  return 0;
}
catch (Error& error) {
  cerr << "pcm: error" << error << endl;
  return -1;
}
catch (const char* error) {
  cerr << "pcm: error " << error << endl;
  return -1;
}

using namespace Pulsar;

SystemCalibrator* time_variation_based (const char* binfile, unsigned nbin) try
{
  ReceptionCalibrator* model = new ReceptionCalibrator (model_type);

  if (measure_cal_V)
    cerr << "pcm: allowing CAL Stokes V to vary" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes V = 0" << endl;

  model->measure_cal_V = measure_cal_V;

  if (measure_cal_Q)
    cerr << "pcm: allowing CAL Stokes Q to vary" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes Q = 0" << endl;

  model->measure_cal_Q = measure_cal_Q;

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model->set_normalize_by_invariant( normalize_by_invariant );

  if (independent_gains)
    cerr << "pcm: each observation has a unique gain" << endl;

  model->independent_gains = independent_gains;

  if (physical_coherency)
    cerr << "pcm: enforcing physically realizable Stokes parameters" << endl;
  else
    cerr << "pcm: risking physically unrealizable Stokes parameters" << endl;

  model->physical_coherency = physical_coherency;

  model->reflections = reflections;

  // add the specified phase bins
  for (unsigned ibin=0; ibin<phase_bins.size(); ibin++)
    model->add_state (phase_bins[ibin]);

  cerr << "pcm: set calibrators" << endl;
  model->set_calibrators (calibrator_filenames);

  if (model->get_nstate_pulsar() == 0)
  {
    // archive from which pulse phase bins will be chosen
    Reference::To<Pulsar::Archive> autobin;

    if (binfile) try 
    {
      autobin = Pulsar::Archive::load (binfile);
      reflections.transform (autobin);

      autobin->fscrunch ();
      autobin->tscrunch ();
      autobin->convert_state (Signal::Stokes);
      autobin->remove_baseline ();
      autobin->dedisperse ();
      autobin->centre (0.0);

      auto_select (*model, autobin, maxbins);

      if (alignment_threshold)
	phase_std = autobin->get_Profile (0,0,0);
    }
    catch (Error& error)
    {
      throw Error (InvalidState, "pcm:mode A",
		   "could not load constraint archive %s:\n\t%s",
		   binfile, error.warning().c_str());
    }
    else
      range_select (*model, phmin, phmax, nbin, maxbins);
      
    cerr << "pcm: " << model->get_nstate_pulsar() << " states" << endl;
    if ( model->get_nstate_pulsar() == 0 )
      throw Error (InvalidState, "pcm:mode A",
		   "no pulsar phase bins have been selected");
  }

  return model;
}
catch (Error& error)
{
  throw error += "pcm:mode A";
}

SystemCalibrator* matrix_template_matching_based (const char* stdname)
{
  PulsarCalibrator* model = new PulsarCalibrator (model_type);

  if (maxbins_set)
    model->set_maximum_harmonic (maxbins);

  model->set_choose_maximum_harmonic (choose_maximum_harmonic);
  model->set_solve_each (solve_each);

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model->set_normalize_by_invariant( normalize_by_invariant );

  if (tim_file)
  {
    FILE* fptr = fopen (tim_file, "w");
    if (!fptr)
      throw Error (InvalidState, "pcm:mode B",
		   "could not open '%s': %s\n", tim_file, strerror(errno));
    model->set_tim_file (fptr);
  }

  Reference::To<Archive> standard;

  cerr << "pcm: loading and setting standard" << endl;

  standard = Archive::load (stdname);
  standard->convert_state (Signal::Stokes);
  reflections.transform (standard);

  RealTimer clock;

  clock.start();

  model->set_standard (standard);

  clock.stop();
  cerr << "pcm: standard set in " << clock << endl;

  cerr << "pcm: adding calibrators" << endl;

  for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
    model->add_observation( Archive::load (calibrator_filenames[ical]) );

  return model;
}

