/***************************************************************************
 *
 *   Copyright (C) 2003 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/psrchive.h"
#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/RobustStepFinder.h"

#include "Pulsar/ManualVariableTransformation.h"
#include "Pulsar/ManualPolnCalibrator.h"

#include "Pulsar/SystemCalibratorUnloader.h"

#include "Pulsar/Database.h"
#include "Pulsar/StandardPrepare.h"

#include "Pulsar/ReceptionModelSolveMEAL.h"
#if HAVE_GSL
#include "Pulsar/ReceptionModelSolveGSL.h"
#endif

#include "MEAL/Depolarizer.h"
#include "MEAL/Polynomial.h"

#include "Pulsar/Interpreter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include "Pulsar/SingleAxis.h"
#include "Pulsar/VariableBackend.h"
#include "Pulsar/ReflectStokes.h"
#include "Pulsar/ProjectionCorrection.h"
#include "Pulsar/Receiver.h"

#include "RealTimer.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

#if HAVE_PGPLOT
#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/StokesSpherical.h"
#include <cpgplot.h>
#endif

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace std;
using namespace Pulsar;

//! Polarimetric Calibration Modelling
class pcm: public Pulsar::Application
{
public:

  //! Default constructor
  pcm ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Unload the total
  void finalize ();

  //! Set the verbosity level
  void set_verbosity (int level);

  //! Add to the list of calibrator databases
  void add_calibrator_database (const string& filename);

  //! Set the model of the instrumental response
  void set_model (const string& file_or_name);

  void add_equation_config (const string& text);
  void load_equation_config (const string& text);
    
  //! Load an impurity model from filename
  void set_impurity (const string& filename);

  //! Load projection transformations from filename
  void set_projection (const string& filename);

  //! Enable the named diagnostic
  void enable_diagnostic (const string& name);

  //! Add parameter to be modelled with unique value for each calibrator
  void set_foreach_cal (const string& code);

  //! Add parameter to be modelled with a step at each calibrator
  void set_stepeach_cal (const string& code);

  //! Add a step in the specified parameter at the specified time
  void add_step_mjd (const string& code);

  //! Add a polynomial variation of a specified parameter
  void add_variation (const string& text);

  //! Set the policy used to select phase bins
  void set_selection_policy (const string& text);

  //! Add the specified phase bin to the constraints
  void add_phase_bin (const string& text);

  //! Set the range of pulse phase to use as constraints
  void set_phase_range (const string& text);

  //! Assume that the specified parameter is not degenerate
  void assume_not_degenerate (const string& text);

  void do_reparallactify (Archive* archive);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  void set_alignment_threshold (const string& arg);
  void load_calibrator_database ();
  void get_span ();
  vector<MJD> get_mjds ();
  void print_time_variation (SystemCalibrator* model);

};

Reference::To<Pulsar::StandardOptions> standard_options;

pcm::pcm () : Pulsar::Application ("pcm", 
				   "polarimetric calibration modelling")
{
  has_manual = true;
  sort_filenames = true;

  add( standard_options = new Pulsar::StandardOptions );
}

// Construct a calibrator model for MEM mode
SystemCalibrator* measurement_equation_modeling (const string& binname,
                                                 unsigned nbin);

// Construct a calibrator model for METM mode
SystemCalibrator* matrix_template_matching (const string& stdname);

// Plot the various components of the model
void plot_state (SystemCalibrator* model, const string& state);

// Print the variations of the Jones matrices
void print_time_variation (SystemCalibrator* model);

#if HAVE_PGPLOT

void plot_chosen (Pulsar::Archive* archive, const vector<unsigned>& bins,
                  string base = "chosen")
{
  string device = base + ".ps/cps";
  cpgbeg (0, device.c_str(), 0, 0);

  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  cerr << "pcm: plotting chosen phase bins" << endl;
  Pulsar::StokesSpherical plot;
  plot.plot (archive);

  cpgswin (0,1,0,1);
  cpgsls (2);

  for (unsigned ibin=0; ibin < bins.size(); ibin++)
  {
    float phase = float(bins[ibin])/float(archive->get_nbin());
    cpgmove (phase, 0);
    cpgdraw (phase, 1);
  }

  cpgend();
}

void plot_onpulse (Pulsar::ReceptionCalibrator& model, Pulsar::Archive* archive)
{
  cpgbeg (0, "onpulse.ps/CPS", 0, 0);

  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  cerr << "pcm: plotting on-pulse phase bins" << endl;
  Pulsar::StokesSpherical plot;
  plot.get_flux()->set_selection( model.get_onpulse() );
  plot.plot (archive);

  cpgend();
}

#endif

Reference::To<Calibration::StandardPrepare> prepare;

void auto_select (Pulsar::ReceptionCalibrator& model,
                  Pulsar::Archive* archive,
                  unsigned maxbins)
{
  cerr << "pcm: choosing up to " << maxbins << " pulse phase bins" << endl;
  vector<unsigned> bins;

  if (!prepare)
    throw Error (InvalidState, "auto_select", 
    "StandardPrepare policy not set");

  prepare->set_input_states (maxbins);
  prepare->choose (archive);
  prepare->get_bins (bins);

  sort (bins.begin(), bins.end());

  for (unsigned ibin=0; ibin < bins.size(); ibin++)
  {
    // cerr << "pcm: adding phase bin " << bins[ibin] << endl;
    model.add_state (bins[ibin]);
  }

  archive->fscrunch ();
  archive->tscrunch ();
  prepare->prepare (archive);

  model.set_standard_data( archive );

#if HAVE_PGPLOT
  plot_chosen (archive, bins);
  plot_onpulse (model, archive);
#endif
}

void range_select (vector<unsigned>& bins,
                   float phmin, float phmax,
                   unsigned nbin, unsigned maxbins)
{
  if (phmin > phmax)
    phmax += 1.0;

  float increment = (phmax - phmin) / (maxbins - 1);

  unsigned last_bin = unsigned (phmax * nbin);

  for (float bin = phmin; bin<=phmax; bin += increment)
  {
    unsigned ibin = unsigned (bin * nbin) % nbin;

    if (ibin != last_bin)
    {
      cerr << "pcm: adding phase bin " << ibin << endl;
      bins.push_back (ibin);
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
                       unsigned nchan, unsigned only_chan=0)
{
  unsigned ichan = 1;

  if (nchan == 1)
    ichan = only_chan;

  const unsigned nstate = plotter.get_calibrator()->get_nstate();

  for (; ichan < nchan; ichan++)
  {
    // don't try to plot if the equation for this channel has no data
    if (plotter.get_calibrator()->get_ndata (ichan) == 0)
      continue;

    char filename [256];
    sprintf (filename, "channel_%d.ps/CPS", ichan);
    cpgbeg (0, filename, 0, 0);

    if (plotter.use_colour)
      cpgsvp (.15,.9, .15,.9);
    else
      cpgsvp (.25,.75,.15,.95);

    bool page = false;

    if (plotter.get_calibrator()->has_cal())
    {
      plotter.plot_cal_constraints (ichan);
      page = true;
    }

    // cerr << "pcm: nstate=" << nstate << endl;
    for (unsigned istate=0; istate<nstate; istate++)
    {
      if (!plotter.get_calibrator()->get_state_is_pulsar (istate))
        continue;

      if (page)
        cpgpage();

      // cerr << "ichan=" << ichan << " istate=" << plot_state << endl;
      plotter.plot_psr_constraints (ichan, istate);
      page = true;
    }

    cpgend ();
  }
}

#endif // HAVE_PGPLOT

// name of the default parametrization
Reference::To<Pulsar::Calibrator::Type> model_type =
                                        new Pulsar::CalibratorTypes::van04_Eq18;

// unloads the solution(s)
Pulsar::SystemCalibrator::Unloader unloader;

// verbosity flags
bool verbose = false;

// The maximum number of bins to use
unsigned maxbins = 16;

// The pulse phase window to use
float phmin = 0, phmax = 0;

// The phase bins to add to the model
vector<unsigned> phase_bins;

// Flag raised when software may choose the maximum harmonic
bool choose_maximum_harmonic = false;

// Mode B: Solve the measurement equation for each observation
bool solve_each = false;

// Mode B: Share a single phase estimate between all observations
bool shared_phase = false;

// significance of phase shift required to fail test
float alignment_threshold = 4.0; // sigma

// significance of phase shift required to automatically rotate in phase
float auto_alignment_threshold = 0.0; // sigma

// total intensity profile of first archive, used to check for phase jumps
Reference::To<Pulsar::Profile> phase_std;

// names of the calibrator files
vector<string> calibrator_filenames;

// Each flux calibrator observation may have unique values of I, Q & U
bool multiple_flux_calibrators = false;

// Model the difference between FluxCalOn and FluxCalOff observations
bool model_fluxcal_on_minus_off = false;

// Derive first guess of calibrator Stokes parameters from fluxcal solution
bool use_fluxcal_stokes = false;

bool degenerate_V_boost = true;
bool degenerate_V_rotation = true;

bool measure_cal_V = true;
bool measure_cal_Q = true;

bool equal_ellipticities = false;

bool normalize_by_invariant = false;
bool independent_gains = false;
bool step_after_cal = false;
bool refcal_through_frontend = true;
bool physical_coherency = false;

float retry_chisq = 0.0;
float invalid_chisq = 0.0;

// filename of previous pcm solution to be used as first guess
string previous_solution;

// set of parameter indeces to be copied from previous_solution
/* if not specified, all parameters are copied */
// string copy_parameters; /* not implemented */

// set of response parameter indeces to be held fixed
string response_fixed;

int main (int argc, char **argv)
{
#ifdef _DEBUG
  size_t in = Reference::Able::get_instance_count();
#endif

  pcm program;
  int ret = program.main (argc, argv);

#ifdef _DEBUG
  size_t out = Reference::Able::get_instance_count();
  cerr << "Leaked: " << out - in << endl;
#endif

  return ret;
}

Reference::To< Pulsar::VariableTransformation > projection;
Reference::To< MEAL::Real4 > impurity;
Reference::To< MEAL::Complex2 > response;

double ionospheric_rm = 0;

Reference::To< MEAL::Univariate<MEAL::Scalar> > gain_variation;
Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_gain_variation;
Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_phase_variation;

//! Temporal variation of response parameters
map< unsigned, Reference::To<MEAL::Univariate<MEAL::Scalar> > > response_variation;

bool get_time_variation ()
{
  return gain_variation || diff_gain_variation || diff_phase_variation
         || (response_variation.size() != 0);
}

string get_string (char code)
{
  switch (code) {
  case 'g': return "gain";
  case 'b': return "diffgain";
  case 'r': return "diffphase";
  default: return "all";
  }
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
  default:
    int index = code - '0';
    if (index >=0 && index <= 9)
    {
      cerr << "reception model parameter " << index << endl;
      response_variation[index] = function;
      return;
    }
  }
  throw Error (InvalidParam, "set_time_variation",
               "unrecognized PAR code = %c", code);
}

vector<MJD> gain_steps;
vector<MJD> diff_gain_steps;
vector<MJD> diff_phase_steps;

void add_step (char code, const MJD& mjd)
{
  switch (code) {
  case 'g':
    cerr << "gain" << endl;
    gain_steps.push_back (mjd);
    return;
  case 'b':
    cerr << "differential gain" << endl;
    diff_gain_steps.push_back (mjd);;
    return;
  case 'r':
    cerr << "differential phase" << endl;
    diff_phase_steps.push_back (mjd);;
    return;
  case 'a':
    cerr << "all backend parameters" << endl;
    gain_steps.push_back (mjd);;
    diff_gain_steps.push_back (mjd);;
    diff_phase_steps.push_back (mjd);;
    return;
  }
  throw Error (InvalidParam, "set_time_variation",
               "unrecognized PAR code = %c", code);
}

struct flags
{
  bool gain;
  bool diff_gain;
  bool diff_phase;

  flags () { gain = diff_gain = diff_phase = false; }
  
  bool get () { return gain || diff_gain || diff_phase; }

  void set (char code)
  {
    switch (code)
      {
      case 'g':
	cerr << "gain" << endl;
	gain = true;
	return;
      case 'b':
	cerr << "differential gain" << endl;
	diff_gain = true;
	return;
      case 'r':
	cerr << "differential phase" << endl;
	diff_phase = true;
	return;
      case 'a':
	cerr << "all backend parameters" << endl;
	gain = true;
	diff_gain = true;
	diff_phase = true;
	return;
      }
    throw Error (InvalidParam, "set",
		 "unrecognized PAR code = %c", code);
  }

  void set_infit (Calibration::SingleAxis* xform)
  {
    xform->set_infit (0, gain);
    xform->set_infit (1, diff_gain);
    xform->set_infit (2, diff_phase);
  }

};

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

static bool output_report = false;
static bool prefit_report = false;
static bool failed_report = false;
static bool input_data = false;

static bool plot_guess = false;
static bool plot_residual = false;
static bool plot_total = false;
static bool plot_result = false;
static bool print_variation = false;

static bool publication_plots = false;

static unsigned solver_verbosity = 0;

void pcm::enable_diagnostic (const string& name)
{
  if (name == "prefit")
    prefit_report = true;

  else if (name == "input")
    input_data = true;

  else if (name == "report")
    output_report = true;

  else if (name == "guess")
    plot_guess = true;

  else if (name == "residual")
    plot_residual = true;

  else if (name == "total")
    plot_total = true;

  else if (name == "result")
    plot_result = true;

  else if (name == "solver")
    solver_verbosity = 1;

  else if (name == "temporal")
  {
    cerr << "pcm: will print temporal variations" << endl;
    print_variation = true;
  }
  else if (name == "failed")
    failed_report = true;

  else
  {
    cerr << "pcm: unrecognized diagnostic name '" << name << "'" << endl;
    exit (-1);
  }
}

// names of files containing a Calibration Database
vector<string> cal_dbase_filenames;

// name of file containing the calibrated template
string template_filename;

// hours from mid-time within which PolnCal observations will be selected
float polncal_hours = 12.0;

// days from mid-time within which FluxCalOn observations will be selected
float fluxcal_days = 7.0;

// look for PolnCal observations with nearby sky coordinates
bool check_coordinates = true;

bool must_have_cals = true;

// threshold used to reject outliers while computing CAL levels
float cal_outlier_threshold = 0.0;

// threshold used to reject CAL observations with no signal
float cal_intensity_threshold = 1.0; // sigma

// minimum degree of polarization of CAL observations
float cal_polarization_threshold = 0.5;  // 50%

// threshold used to insert steps in model
float step_threshold = 0.0;

// name of file containing list of calibrator Archive filenames
string calfile;

/* Flux calibrator solution from which first guess of calibrator Stokes
   parameters will be derived */
Reference::To<Pulsar::FluxCalibrator> flux_cal;

void load_calibrator_database ();

// Number of threads used to solve equations
unsigned nthread = 1;

// name of file containing list of filenames to be calibrated
string calibrate_these;

// name of file from which phase bins will be chosen
string binfile;

// name of least squares minimization algorithm
string least_squares;

// name of file containing MEAL::Function text interface commands
vector<string> equation_configuration;

bool unload_each_calibrated = true;
bool fscrunch_data_to_template = false;

bool reparallactify = false;

string unload_path;
string output_filename = "pcm.fits";

void pcm::set_verbosity (int level)
{
  verbose = true;

  if (level > 4)
    Calibration::ReceptionModel::very_verbose = true;

  if (level > 3) 
    Calibration::ReceptionModel::verbose = true;

  if (level > 2)
    Calibration::SignalPath::verbose = true;

  if (level > 1)
    Pulsar::Archive::set_verbosity (level-1);

  Pulsar::Calibrator::verbose = level;
}

void pcm::set_alignment_threshold (const string& arg)
{
  if (arg[0] == '@')
    auto_alignment_threshold = atof (arg.c_str()+1);
  else
    alignment_threshold = atof (arg.c_str());
}

void pcm::add_calibrator_database (const string& arg)
{
  cal_dbase_filenames.push_back (arg);
}

void pcm::set_model (const string& filename)
{
  try
    {
      response = Pulsar::load_transformation (filename);
      cerr << "pcm: response model loaded from " << filename << endl;
      return;
    }
  catch (Error& error)
    {
      if (verbose)
	cerr << "pcm: error" << error << endl;
    }

  model_type = Pulsar::Calibrator::Type::factory (filename);
}

void pcm::set_impurity (const string& filename)
{
  cerr << "pcm: loading impurity transformation from " << filename << endl;
  impurity = MEAL::Function::load<MEAL::Real4> (filename);
}

void pcm::set_projection (const string& filename)
{
  cerr << "pcm: loading projection transformations from " << filename << endl;

  ManualPolnCalibrator* cal = new ManualPolnCalibrator (filename);
  projection = new ManualVariableTransformation (cal);
}

flags foreach_calibrator;
flags stepeach_calibrator;

void pcm::set_foreach_cal (const string& code)
{
  cerr << "pcm: for each calibrator, a unique value of ";
  foreach_calibrator.set( code[0] );
}


void pcm::set_stepeach_cal (const string& code)
{
  cerr << "pcm: at each calibrator, a step in ";
  stepeach_calibrator.set( code[0] );

  if (code[1]==':' && code[2]=='A')
    step_after_cal = true;

  cerr << "pcm: assuming cals are observed "
       << (step_after_cal ? "after" : "before") << " pulsars" << endl;
}

void pcm::add_step_mjd (const string& text)
{
  char code;
  char dummy;
  double mjd;

  istringstream is (text);
  is >> code >> dummy >> mjd;

  if (is.bad())
    throw Error (InvalidParam, "pcm",
		 "error parsing '"+text+"' as PAR:MJD");

  MJD epoch (mjd);

  cerr << "pcm: inserting a step in ";
  add_step( code, epoch );
  cerr << " at MJD=" << epoch << endl;
}

void pcm::add_variation (const string& text)
{
  char code;
  unsigned order;
  if( sscanf (text.c_str(), "%c:%u", &code, &order) != 2 )
    throw Error (InvalidParam, "pcm",
		 "error parsing '" + text + "' as PAR:N");

  cerr << "pcm: using a polynomial of degree " << order << " to model ";
  set_time_variation( code, new MEAL::Polynomial (order+1) );
}

void pcm::set_selection_policy (const string& text)
{
  prepare = Calibration::StandardPrepare::factory (text);
}

void pcm::add_phase_bin (const string& text)
{
  unsigned bin = fromstring<unsigned> (text);
  cerr << "pcm: adding phase bin " << bin << endl;
  phase_bins.push_back (bin);
}

void pcm::set_phase_range (const string& text)
{
  char dummy;
  if (sscanf (text.c_str(), "%f%c%f", &phmin, &dummy, &phmax) != 3)
    throw Error (InvalidParam, "pcm",
		 "error parsing " + text + " as phase range");

  cerr << "pcm: selecting input states from " << phmin << " to " << phmax
       << endl;
}

void pcm::add_equation_config (const string& text)
{
  separate (text, equation_configuration, ",");
}

void pcm::load_equation_config (const string& text)
{
  loadlines (text, equation_configuration);
}

void pcm::assume_not_degenerate (const string& text)
{
  if (text.find ('b') != string::npos)
  {
    cerr << "pcm: assuming that Stokes V boost is not degenerate" << endl;
    degenerate_V_boost = false;
  }
  
  if (text.find ('r') != string::npos)
  {
    cerr << "pcm: assuming that Stokes V rotation is not degenerate" << endl;
    degenerate_V_rotation = false;
  }
}

//! Add command line options
void pcm::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  //! Remove the -q, -v and -V (quiet, verbose and very verbose) options
  arg = menu.find ("q");
  if (arg)
    menu.remove (arg);
  
  arg = menu.find ("v");
  if (arg)
    menu.remove (arg);
  
  arg = menu.find ("V");
  if (arg)
    menu.remove (arg);

  arg = menu.add (this, &pcm::set_verbosity, 'V', "level");
  arg->set_help ("set verbosity level [0->4]");
  
  menu.add ("\n" "Output options:");

  arg = menu.add (&unloader, &SystemCalibrator::Unloader::set_archive_class,
		  'A', "class");
  arg->set_help ("set the output archive class name");

  arg = menu.add (unload_path, 'O', "path");
  arg->set_help ("set directory to which outputs are written");

  arg = menu.add (output_filename, "out", "fname");
  arg->set_help ("set cal solution output filename "
		 "(default=" + output_filename + ")");

  arg = menu.add (unload_each_calibrated, 'N');
  arg->set_help ("do not unload calibrated data files");

  menu.add ("\n" "Input options:");

  arg = menu.add (calfile, 'C', "file");
  arg->set_help ("filename with list of calibrator files");

  arg = menu.add (this, &pcm::add_calibrator_database, 'd', "file");
  arg->set_help ("add file to list of calibrator databases");

  arg = menu.add (calibrate_these, 'W', "file");
  arg->set_help ("filename with list of other data files to be calibrated");

  arg = menu.add (fluxcal_days, 'F', "days");
  arg->set_help ("use flux calibrators within days of pulsar data mid-time");

  arg = menu.add (polncal_hours, 'L', "hours");
  arg->set_help ("use reference sources within hours of pulsar data mid-time");

  arg = menu.add (must_have_cals, 'w');
  arg->set_help ("continue if no calibrators are found");

  arg = menu.add (ProjectionCorrection::trust_pointing_feed_angle, 'y');
  arg->set_help ("always trust the Pointing::feed_angle attribute");

  arg = menu.add (check_coordinates, 'Z');
  arg->set_help ("ignore the sky coordinates of PolnCal observations");

  arg = menu.add (previous_solution, "solution", "file");
  arg->set_help ("load previous solution from 'file' as first guess");
 
#if 0 
  /* not yet implemented */
  arg = menu.add (copy_parameters, "copy", "i,j,k");
  arg->set_help ("copy only specified parameters from previous solution");
#endif

  arg = menu.add (response_fixed, "fix", "i,j,k");
  arg->set_help ("hold specified parameters fixed");

  menu.add ("\n" "Outlier and step detection options:");

  arg = menu.add (cal_outlier_threshold, 'K', "sigma");
  arg->set_help ("Reject outliers when computing CAL levels");

  arg = menu.add (cal_intensity_threshold, "calI", "sigma");
  arg->set_help ("Minimum significance of CAL intensity");

  arg = menu.add (cal_polarization_threshold, "calp", "frac");
  arg->set_help ("Minimum degree of polarization of CAL");

  arg = menu.add (step_threshold, "step", "sigma");
  arg->set_help ("Insert steps where adjacent CAL levels differ");

  menu.add ("\n" "General model configuration options:");

  arg = menu.add (this, &pcm::set_model, 'm', "model");
  arg->set_help ("receiver model name: e.g. bri00e19 or van04e18 [default]");

  arg = menu.add (this, &pcm::add_equation_config, 'e', "cmd");
  arg->set_help ("add measurement equation configuration option");
  
  arg = menu.add (this, &pcm::load_equation_config, 'E', "file");
  arg->set_help ("load measurement equation configuration options");
  
  arg = menu.add (this, &pcm::set_impurity, 'I', "file");
  arg->set_help ("load impurity transformation from file");

  arg = menu.add (this, &pcm::set_projection, 'P', "file");
  arg->set_help ("load projection transformations from file");

  arg = menu.add (reparallactify, "repara");
  arg->set_help ("reparallactify the input data");

  arg = menu.add (ionospheric_rm, "iono", "rm");
  arg->set_help ("ionospheric Faraday rotation measure");

  arg = menu.add (least_squares, 'l', "solver");
  arg->set_help ("solver: MEAL [default] or GSL");

  arg = menu.add (nthread, 't', "ncore");
  arg->set_help ("solve using ncore threads");

  arg = menu.add (use_fluxcal_stokes, 'x');
  arg->set_help ("estimate calibrator Stokes parameters using fluxcal");

  arg = menu.add (refcal_through_frontend, 'Q');
  arg->set_help ("reference source coupled after frontend");

  arg = menu.add (retry_chisq, 'R', "gof");
  arg->set_help ("retry solving channels with reduced chisq above gof");

  arg = menu.add (invalid_chisq, 'X', "gof");
  arg->set_help ("flag invalid channels with reduced chisq above gof");

  arg = menu.add (this, &pcm::enable_diagnostic, 'D', "name");
  arg->set_help ("enable diagnostic: name=report,guess,residual,result,total");

  menu.add ("\n" "MEM: Measurement Equation Modeling \n"
	    "  -- observations of an unknown source (van Straten 2004)\n");

  string par_help =
    "PAR can be one of \n"
    " g = absolute gain \n"
    " b = differential gain \n"
    " r = differential phase \n"
    " a = all of the above";
    
  arg = menu.add (this, &pcm::set_foreach_cal, 'U', "PAR");
  arg->set_help ("model PAR with a unique value for each CAL");
  arg->set_long_help (par_help);

  string where_help = "\n\n"
    "W can be one of \n"
    " A = cals are observed after pulsars \n"
    " B = cals are observed before pulsars (default) ";

  arg = menu.add (this, &pcm::set_stepeach_cal, 'u', "PAR[:W]");
  arg->set_help ("model PAR with a step at each CAL");
  arg->set_long_help (par_help + where_help);

  arg = menu.add (this, &pcm::add_step_mjd, 'i', "PAR:MJD");
  arg->set_help ("model PAR with a step at the given epoch (MJD)");
  arg->set_long_help (par_help);

  arg = menu.add (this, &pcm::add_variation, 'o', "PAR:N");
  arg->set_help ("model PAR as N degree polyomial");
  arg->set_long_help (par_help);

  arg = menu.add (this, &pcm::set_selection_policy, 'B', "choose");
  arg->set_help ("set the phase bin selection policy: int, pol, orth, inv");
  arg->set_long_help ("separate multiple policies with commas");

  arg = menu.add (binfile, 'c', "file");
  arg->set_help ("choose best input states from observation in file");

  arg = menu.add (this, &pcm::add_phase_bin, 'b', "nbin");
  arg->set_help ("add phase bin to constraints");

  arg = menu.add (maxbins, 'n', "nbin");
  arg->set_help ("set the number of phase bins to choose as input states");

  arg = menu.add (this, &pcm::set_phase_range, 'p', "pA,pB");
  arg->set_help ("set the phase window from which to choose input states");

  arg = menu.add (this, &pcm::set_alignment_threshold, 'a', "bins");
  arg->set_help ("set the threshold for testing input data phase alignment");

  arg = menu.add (normalize_by_invariant, 's');
  arg->set_help ("normalize Stokes parameters by total invariant interval");

  arg = menu.add (independent_gains, 'g');
  arg->set_help ("unique absolute gain for each pulsar observation [DEVEL]");

  arg = menu.add (physical_coherency, 'r');
  arg->set_help ("enforce physically realizable Stokes parameters [DEVEL]");

  menu.add ("\n" 
	    "Constraints on degeneracy under commutation \n"
	    "  -- See " PSRCHIVE_HTTP "/manuals/pcm/degeneracy.shtml \n");

  arg = menu.add (measure_cal_Q, 'q');
  arg->set_help ("assume that CAL Stokes Q = 0 (linear feeds only)");

  arg = menu.add (measure_cal_V, 'v');
  arg->set_help ("assume that CAL Stokes V = 0 (linear feeds only)");

  arg = menu.add (equal_ellipticities, 'k');
  arg->set_help ("assume that the receptors have equal ellipticities");

  arg = menu.add (model_fluxcal_on_minus_off, 'Y');
  arg->set_help ("model the difference between FluxCal-On and FluxCal-Off");

  arg = menu.add (multiple_flux_calibrators, 'f');
  arg->set_help ("model each FluxCal with unique Stokes parameters");

  arg = menu.add (this, &pcm::assume_not_degenerate, 'T', "code");
  arg->set_help ("assume Stokes V [b]oost and/or [r]otation is not degenerate");
  
  menu.add ("\n"
	    "METM: Measurement Equation Template Matching\n"
	    "  -- observations of a known source as in van Straten (2013) \n");

  arg = menu.add (template_filename, 'S', "file");
  arg->set_help ("filename of calibrated standard");

  arg = menu.add (fscrunch_data_to_template, 'G');
  arg->set_help ("fscrunch data to match number of channels of standard");

  arg = menu.add (choose_maximum_harmonic, 'H');
  arg->set_help ("automatically choose the number of harmonics");

  arg = menu.add (maxbins, 'n', "nharm");
  arg->set_help ("set the number of harmonics to use as input states");

  arg = menu.add (solve_each, '1');
  arg->set_help ("solve independently for each observation");

  arg = menu.add (shared_phase, 'z');
  arg->set_help ("share a single phase shift estimate b/w all observations");
}

Reference::To<Pulsar::PolnCalibrator> pcm_solution;

void pcm::setup ()
{
  if (nthread == 0)
    throw Error (InvalidState, "pcm",
		 "invalid number of threads = %u", nthread);

  cerr << "pcm: using a maximum of " << maxbins << " bins or harmonics" 
       << endl;

  bool mem_mode = template_filename.empty();
  
  if (mem_mode && phmin == phmax && binfile.empty())
    throw Error (InvalidState, "pcm",
      "In MEM mode, at least one of the following options"
      " must be specified:\n"
      " -p min,max  Choose constraints from the specified pulse phase range \n"
      " -c archive  Choose optimal constraints from the specified archive");

  if (!previous_solution.empty())
  {
    Reference::To<Archive> cal = Archive::load (previous_solution);
    pcm_solution = new PolnCalibrator (cal);

    const Calibrator::Type* type = pcm_solution->get_type ();

    cerr << "pcm: previous solution has type=" << type->get_name()
	 << " and nparam=" << type->get_nparam() << endl;
    
    if (!type->is_a (model_type) || !model_type->is_a (type))
    {
      cerr << "pcm: over-riding model type=" << model_type->get_name() << endl;
      model_type = type->clone();
    }
  }
  
  if (mem_mode && fscrunch_data_to_template)
    throw Error (InvalidState, "pcm",
		 "In MEM mode, the -G option is not supported");

  if (!mem_mode)
    alignment_threshold = 0.0;

  if (!calfile.empty())
    stringfload (&calibrator_filenames, calfile);

  load_calibrator_database();

  if (!prepare)
  {
    Calibration::MultipleRanking* mult = new Calibration::MultipleRanking;
    mult->add( new Calibration::MaximumPolarization );
    mult->add( new Calibration::MaximumIntensity );
    prepare = mult;
  }

  unloader.set_program ( "pcm" );
  unloader.set_filename( output_filename );
}

  
Reference::To<Pulsar::SystemCalibrator> model;
Reference::To<Pulsar::Archive> total;
Reference::To<Pulsar::Archive> archive;

void configure_model (Pulsar::SystemCalibrator* model)
{
  model->set_nthread (nthread);
  model->set_report_projection (true);
  model->set_ionospheric_rotation_measure (ionospheric_rm);

  if (pcm_solution)
    model->set_previous_solution (pcm_solution);
  
  model->set_cal_outlier_threshold (cal_outlier_threshold);
  model->set_cal_intensity_threshold (cal_intensity_threshold);
  model->set_cal_polarization_threshold (cal_polarization_threshold);

  if (step_threshold)
    model->set_step_finder( new RobustStepFinder (step_threshold) );

  model->set_report_initial_state (prefit_report);
  model->set_report_input_data (input_data);
  model->set_report_input_failed (failed_report);

  if (response)
    model->set_response( response );

  if (impurity)
    model->set_impurity( impurity );

  if (projection)
    model->set_projection( projection );

  if (gain_variation)
    model->set_gain( gain_variation );

  if (diff_gain_variation)
    model->set_diff_gain( diff_gain_variation );

  if (diff_phase_variation)
    model->set_diff_phase( diff_phase_variation );

  for (auto ptr : response_variation)
    model->set_response_variation( ptr.first, ptr.second );

  vector<unsigned> fixed_indeces;
  while (!response_fixed.empty())
  {
    string sub = stringtok (response_fixed, ", ");
    fixed_indeces.push_back ( fromstring<unsigned>(sub) );
  }

  if (fixed_indeces.size())
  {
    cerr << "pcm: fixing response at iparam=";
    for (auto element: fixed_indeces)
      cerr << element << " ";
    cerr << endl;
    model->set_response_fixed (fixed_indeces);
  }
  
  if (foreach_calibrator.get())
  {
    Reference::To< Calibration::SingleAxis > foreach;
    foreach = new Calibration::SingleAxis;
    foreach_calibrator.set_infit (foreach);
    model->set_foreach_calibrator (foreach);
  }
  
  if (stepeach_calibrator.get())
  {
    Reference::To< Calibration::VariableBackend > stepeach;
    stepeach = new Calibration::VariableBackend;
    stepeach_calibrator.set_infit (stepeach->get_backend());
    model->set_stepeach_calibrator (stepeach);
  }

  for (unsigned i=0; i < gain_steps.size(); i++)
    model->add_gain_step (gain_steps[i]);
  
  for (unsigned i=0; i < diff_gain_steps.size(); i++)
    model->add_diff_gain_step (diff_gain_steps[i]);

  for (unsigned i=0; i < diff_phase_steps.size(); i++)
    model->add_diff_phase_step (diff_phase_steps[i]);

  model->set_step_after_cal( step_after_cal );

  if (refcal_through_frontend)
    cerr << "pcm: reference source illuminates frontend" << endl;
  else
    cerr << "pcm: reference source coupled after frontend" << endl;

  model->set_refcal_through_frontend( refcal_through_frontend );

  if (!least_squares.empty())
    model->set_solver( new_solver(least_squares) );

  model->get_solver()->set_verbosity( solver_verbosity );

  if (retry_chisq)
    model->set_retry_reduced_chisq( retry_chisq );

  if (invalid_chisq)
    model->set_invalid_reduced_chisq( invalid_chisq );

  model->set_equation_configuration( equation_configuration );
}

void check_phase (Pulsar::Archive* archive)
{
  Reference::To<Pulsar::Archive> temp = archive->total();
  Estimate<double> shift = temp->get_Profile(0,0,0)->shift (*phase_std);

  double abs_shift = fabs( shift.get_value() );
  
  if ( auto_alignment_threshold &&
       abs_shift > auto_alignment_threshold * shift.get_error() )
  {
    cerr << "pcm: phase shifting observation to match reference" << endl;
    archive->rotate_phase( shift.get_value() );
  }

  else if ( alignment_threshold &&
	    abs_shift > 1.0 / phase_std->get_nbin() &&
	    abs_shift > alignment_threshold * shift.get_error() )
  {
    /* if the shift is greater than 1 phase bin and significantly
       more than the error, then there may be a problem */

    Error error (InvalidParam, "pcm");
    error <<
      "ERROR apparent phase shift between input archives\n"
      "\tshift = " << shift.get_value() << " +/- " << shift.get_error () <<
      "  =  " << int(shift.get_value() * phase_std->get_nbin()) <<
      " phase bins";
    
    throw error;
  }
}

void pcm::do_reparallactify (Pulsar::Archive* archive)
{
  Pulsar::Receiver* rcvr = archive->get<Receiver>();
  if (!rcvr)
    throw Error (InvalidState, "pcm reparallactify",
                 "no Receiver extension available");

  cerr << "pcm: re-parallactifying data" << endl;
  ProjectionCorrection projection;

  rcvr->set_projection_corrected (false);

  projection.set_archive( archive );

  unsigned nsub = archive->get_nsubint();
  for (unsigned isub=0; isub < nsub; isub++)
  {
    Pulsar::Integration* subint = archive->get_Integration (isub);
    Jones<double> xform = projection (isub);

    // the returned matrix transforms from the corrected to the observed
    subint->expert()->transform (xform);
  }
}

void pcm::process (Pulsar::Archive* archive)
{
  if (archive->get_type() == Signal::Pulsar)
  {
    if (verbose)
      cerr << "pcm: preparing pulsar data" << endl;

    prepare->prepare (archive);

    if (reparallactify)
      do_reparallactify (archive);
  }

  if (!model)
  {
    cerr << "pcm: creating model" << endl;

    if (!template_filename.empty())
      model = matrix_template_matching (template_filename);
    else
      model = measurement_equation_modeling (binfile, archive->get_nbin());

    configure_model (model);  
  }

  /*
    test for phase shift only if phase_std is not from current archive.
    this test will fail if binfile is a symbollic link.
  */
  if (phase_std && (binfile.empty() || archive->get_filename() != binfile))
  {
    if (verbose)
      cerr << "pcm: creating checking phase" << endl;

    check_phase (archive);
  }

  if ((alignment_threshold || auto_alignment_threshold) && !phase_std)
  {
    cerr << "pcm: creating phase reference" << endl;

    // store an fscrunched and tscrunched clone for phase reference
    Reference::To<Archive> temp = archive->total();
    phase_std = temp->get_Profile (0,0,0);
  }

  if (fscrunch_data_to_template &&
      model->get_nchan() != archive->get_nchan())
  {
    cerr << "pcm: frequency integrating data (nchan=" << archive->get_nchan()
	 << ") to match calibrator (nchan=" << model->get_nchan()
	 << ")" << endl;
    archive->fscrunch_to_nchan (model->get_nchan());
  }

  cerr << "pcm: adding observation" << endl;

  model->preprocess( archive );
  model->add_observation( archive );

  if (archive->get_type() != Signal::Pulsar)
    return;
      
  if (verbose)
    cerr << "pcm: calibrate with current best guess" << endl;

  model->precalibrate (archive);

  if (solve_each)
  {
    string oldname = archive->get_filename();
    string newname = replace_extension (oldname, ".calib");
    archive->unload (newname);    
    cerr << "pcm: unloaded " << newname << endl;
  }

#if 0
  if (verbose)
    cerr << "pcm: add to total" << endl;

  if (!total)
    total = archive;
  else
    total->append (archive);
    
  total->tscrunch ();
#endif

}

void pcm::finalize ()
{
  if (solve_each)
  {
    if (total)
    {
      cerr << "pcm: writing total calibrated pulsar archive" << endl;
      total->unload ("total.ar");
    }
    return;
  }

  if (total)
  {
    cerr << "pcm: writing total uncalibrated pulsar archive" << endl;
    total->unload ("first.ar");
  }

#if HAVE_PGPLOT

  Pulsar::SystemCalibratorPlotter plotter (model);
  plotter.use_colour = !publication_plots;

  try {

    if (plot_guess)
      plot_state (model, "guess");

    if (plot_total && total)
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

    if (plot_residual && model->get_nstate_pulsar())
    {
      cerr << "pcm: plotting pulsar constraints" << endl;
      plot_constraints (plotter, model->get_nchan());
    }

  }
  catch (Error& error)
  {
    cerr << "pcm: error while producing plots ignored" << endl;
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
    return;
  }

  if (model->has_valid())
  {
    cerr << "pcm: unload model" << endl;
    unloader.unload (model);
  }
  else
  {
    cerr << "pcm: no valid solutions to unload" << endl;
  }

  if (print_variation && get_time_variation())
  {
    cerr << "pcm: print variation" << endl;
    print_time_variation (model);
  }

#if HAVE_PGPLOT

  if (plot_result) try
  {
    cerr << "pcm: plot result" << endl;

    plot_state (model, "result");

    if (get_time_variation())
    {
      cpgbeg (0, "result_variations.ps/PS", 0, 0);
      cpgsvp (0.1,.9, 0.1,.9);

      unsigned panels = plotter.npanel;
      plotter.npanel = 4;

      cerr << "pcm: plotting time variation functions" << endl;
      plotter.plot_time_variations ();

      plotter.npanel = panels;
      cpgend ();
    }
  }
  catch (Error& error)
  {
    cerr << "pcm: error while plotting results" << error << endl;
  }

  if (plot_residual && model->get_nstate_pulsar()) try
  {
    plotter.set_plot_residual (true);

    cerr << "pcm: plotting pulsar constraints with model" << endl;
    plot_constraints (plotter, model->get_nchan());
  }
  catch (Error& error)
  {
    cerr << "pcm: error while plotting residual" << error << endl;
  }

#endif // HAVE_PGPLOT

  if (!calibrate_these.empty())
  {
    filenames.clear();
    stringfload (&filenames, calibrate_these);
    cerr << "pcm: calibrating " << filenames.size() << " files listed in "
         << calibrate_these << endl;
  }
  else
  {
    for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
      dirglob (&filenames, calibrator_filenames[ical]);

    cerr << "pcm: calibrating archives (PSR and CAL)" << endl;
  }

  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    if (verbose)
      cerr << "pcm: loading " << filenames[i] << endl;

    archive = Pulsar::Archive::load(filenames[i]);

    cout << "pcm: loaded archive: " << filenames[i] << endl;

    standard_options->process ( archive );

    if (reparallactify && archive->get_type() == Signal::Pulsar)
      do_reparallactify (archive);

    model->precalibrate( archive );

    if (unload_each_calibrated)
    {
      string newname = replace_extension (filenames[i], ".calib");

      if (!unload_path.empty())
        newname = unload_path + "/" + basename (newname);

      if (verbose)
        cerr << "pcm: calibrated Archive name '" << newname << "'" << endl;

      archive->unload (newname);

      cout << "New file " << newname << " unloaded" << endl;
    }

    if (calibrate_these.empty() && archive->get_type() == Signal::Pulsar)
    {
      if (verbose)
        cerr << "pcm: correct and add to calibrated total" << endl;

      if (!total)
        total = archive;
      else
        total->append (archive);

      total->tscrunch ();
    }
  }
  catch (Error& error)
  {
    cerr << error << endl;
  }

  if (total)
  {
    cerr << "pcm: writing total integrated pulsar archive" << endl;
    total->unload ("total.ar");
  }

#if HAVE_PGPLOT

  if (plot_total && total)
  {
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

  if (total && phase_bins.size() != 0)
  {
    total->fscrunch ();
    total->tscrunch ();
    prepare->prepare (total);

    plot_chosen (total, phase_bins, "selected");
  }

#endif // HAVE_PGPLOT

  cerr << "pcm: finished" << endl;
}


using namespace Pulsar;

SystemCalibrator* measurement_equation_modeling (const string& binfile,
                                                 unsigned nbin) try
{
  ReceptionCalibrator* model = new ReceptionCalibrator (model_type);

  model->output_report = output_report;

  if (degenerate_V_boost)
    cerr << "pcm: boost along Stokes V is intrinsically degenerate" << endl;
  else
    cerr << "pcm: boost along Stokes V is not degenerate" << endl;

  model->degenerate_V_boost = degenerate_V_boost;

  if (measure_cal_V)
    cerr << "pcm: if available, will use fluxcal data to constrain"
      " CAL Stokes V" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes V = 0" << endl;

  model->measure_cal_V = measure_cal_V;

  if (degenerate_V_rotation)
    cerr << "pcm: rotation about Stokes V is intrinsically degenerate" << endl;
  else
    cerr << "pcm: rotation about Stokes V is not degenerate" << endl;

  model->degenerate_V_rotation = degenerate_V_rotation;

  if (measure_cal_Q)
    cerr << "pcm: allowing CAL Stokes Q to vary" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes Q = 0" << endl;

  model->measure_cal_Q = measure_cal_Q;

  if (equal_ellipticities)
    cerr << "pcm: assuming that the receptor ellipticities are equal" << endl;

  model->equal_ellipticities = equal_ellipticities;

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

  if (multiple_flux_calibrators)
    cerr <<
      "pcm: each flux calibrator observation "
      "will be independently modeled" << endl;

  model->multiple_flux_calibrators = multiple_flux_calibrators;

  if (model_fluxcal_on_minus_off)
    cerr << "pcm: modeling difference between FluxCalOn and FluxCalOff" << endl;

  model->model_fluxcal_on_minus_off = model_fluxcal_on_minus_off;

  if (flux_cal)
    model->set_flux_calibrator (flux_cal);
  
  cerr << "pcm: set calibrators" << endl;
  model->set_calibrators (calibrator_filenames);
  model->set_calibrator_preprocessor (standard_options);

  // archive from which pulse phase bins will be chosen
  Reference::To<Pulsar::Archive> autobin;

  if (!binfile.empty()) try 
  {
    // archive from which pulse phase bins will be chosen
    Reference::To<Pulsar::Archive> autobin;

    autobin = Archive::load (binfile);

    auto_select (*model, autobin, maxbins);

    if (alignment_threshold)
      phase_std = autobin->get_Profile (0,0,0);
  }
  catch (Error& error)
  {
    error << "\ncould not load constraint archive '" << binfile << "'";
    throw error;
  }

  if (phmin != phmax)
    range_select (phase_bins, phmin, phmax, nbin, maxbins);

  // add the specified phase bins
  for (unsigned ibin=0; ibin<phase_bins.size(); ibin++)
    model->add_state (phase_bins[ibin]);

  cerr << "pcm: " << model->get_nstate_pulsar() << " states" << endl;
  if ( model->get_nstate_pulsar() == 0 )
    throw Error (InvalidState, "pcm:mode A",
                 "no pulsar phase bins have been selected");

  return model;
}
catch (Error& error)
{
  throw error += "pcm:mode A";
}

SystemCalibrator* matrix_template_matching (const string& stdname)
{
  PulsarCalibrator* model = new PulsarCalibrator (model_type);

  if (shared_phase)
    model->share_phase ();

  if (choose_maximum_harmonic)
    model->set_choose_maximum_harmonic ();
  else
    model->set_maximum_harmonic (maxbins);

  if (solve_each)
  {
    SystemCalibrator::Unloader* mtm = new SystemCalibrator::Unloader(unloader);
    mtm->set_extension ("mtm");

    model->set_solve_each (true);
    model->set_unload_each (mtm);
  }

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model->set_normalize_by_invariant( normalize_by_invariant );

  Reference::To<Archive> standard;

  cerr << "pcm: loading and setting standard" << endl;

  standard = Archive::load (stdname);
  standard->convert_state (Signal::Stokes);

  RealTimer clock;

  clock.start();

  model->set_standard (standard);

  clock.stop();
  cerr << "pcm: standard set in " << clock << endl;

  if (calibrator_filenames.size())
    cerr << "pcm: adding " << calibrator_filenames.size() << " calibrators"
         << endl;

  for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
  {
    Archive* cal = Archive::load (calibrator_filenames[ical]);
    standard_options->process (cal);
    model->add_observation (cal);
  }

  return model;
}

static MJD start_time;
static MJD end_time;

void pcm::get_span ()
{
  Pulsar::Profile::no_amps = true;

  static bool loaded = false;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load( filenames[ifile] );
    MJD start = archive->start_time();
    MJD end = archive->end_time();

    if (!loaded || start < start_time)
      start_time = start;
    if (!loaded || end > end_time)
      end_time = end;

    loaded = true;
  }
  catch (Error& error)
  {
    cerr << "pcm: get_span() error while handling " << filenames[ifile]
         << endl << "\t" << error.get_message() << endl;

    filenames.erase( filenames.begin() + ifile );
    ifile --;
  }

  double span = (end_time - start_time).in_days();
  string unit = "days";

  if (span < 1)
  {
    span += 24;
    unit = "hours";
  }

  cerr << "pcm: data span " << span << " " << unit << endl;

  Pulsar::Profile::no_amps = false;
}

/* **********************************************************************

   FIND APPROPRIATE CALIBRATOR OBSERVATIONS IN THE DATABASE

   ********************************************************************** */

void pcm::load_calibrator_database () try
{
  if (!cal_dbase_filenames.size())
    return;

  if (!filenames.size())
    return;

  Reference::To<Pulsar::Archive> archive;
  while (filenames.size()) try
  {
    archive = Pulsar::Archive::load( filenames.front() );
    break;
  }
  catch (Error& error)
  {
    cerr << "load_calibrator_database: error loading " << filenames.front()
         << endl << error << endl;
    filenames.erase( filenames.begin() );
  }

  get_span ();

  MJD mid = 0.5 * (end_time + start_time);
  double span_hours = (end_time - start_time).in_days() * 24.0;
  double search_hours = 0.5*span_hours + polncal_hours;

  Reference::To<Pulsar::Database> database;

  for (unsigned i=0; i<cal_dbase_filenames.size(); i++)
  {
    cout << "pcm: loading database from " << cal_dbase_filenames[i] << endl;
    if (!database)
      database = new Pulsar::Database (cal_dbase_filenames[i]);
    else
      database->load (cal_dbase_filenames[i]);
  }

  cerr << "pcm: database constructed with " << database->size() 
       << " entries" << endl;

  char buffer[256];

  cerr << "pcm: searching for reference source observations"
    " within " << search_hours << " hours of midtime" << endl;

  cerr << "pcm: midtime = "
       << mid.datestr (buffer, 256, "%Y-%m-%d-%H:%M:00") << endl;

  Pulsar::Database::Criteria criteria;
  criteria = database->criteria (archive, Signal::PolnCal);
  criteria.entry->time = mid;
  criteria.check_coordinates = check_coordinates;
  criteria.minutes_apart = search_hours * 60.0;

  vector<const Pulsar::Database::Entry*> oncals;
  database->all_matching (criteria, oncals);

  unsigned poln_cals = oncals.size();

  if (poln_cals == 0)
  {
    cerr << "pcm: no PolnCal observations found; closest match was \n\n"
         << database->get_closest_match_report () << endl;

    if (must_have_cals && calfile.empty())
    {
      cerr << "pcm: cannot continue (disable this check with -)" << endl;
      exit (-1);
    }
  }

  if (use_fluxcal_stokes) try
  {
    flux_cal = database->generateFluxCalibrator (archive);
  }
  catch (Error& error)
  {
    cerr << "pcm: failed to generate FluxCalibrator solution"
         << error << endl;
    exit (-1);
  }

  if (!template_filename.empty())
    cerr << "pcm: no need for flux calibrator observations" << endl;
  else
  {
    double span_days = (end_time - start_time).in_days();
    double search_days = 0.5*span_days + fluxcal_days;

    criteria.check_coordinates = false;
    criteria.minutes_apart = search_days * 24.0 * 60.0;
    criteria.entry->obsType = Signal::FluxCalOn;
    
    cerr << "pcm: searching for on-source flux calibrator observations"
      " within " << search_days << " days of midtime" << endl;

    database->all_matching (criteria, oncals);

    if (oncals.size() == poln_cals)
      cerr << "pcm: no FluxCalOn observations found; closest match was \n\n"
           << database->get_closest_match_report () << endl;

    if (model_fluxcal_on_minus_off)
    {
      unsigned ncals = oncals.size();

      criteria.entry->obsType = Signal::FluxCalOff;

      cerr << "pcm: searching for off-source flux calibrator observations"
              " within " << search_days << " days of midtime" << endl;

      database->all_matching (criteria, oncals);

      if (oncals.size() == ncals)
        cerr << "pcm: no FluxCalOff observations found; closest match was \n\n"
             << database->get_closest_match_report () << endl;
    }
  }

  sort (oncals.begin(), oncals.end());
  
  for (unsigned i = 0; i < oncals.size(); i++)
  {
    string filename = database->get_filename( oncals[i] );
    cerr << "pcm: adding " << oncals[i]->filename << endl;
    calibrator_filenames.push_back (filename);
  }
}
catch (Error& error)
{
  cerr << "pcm: error loading CAL database" << error << endl;
  exit (-1);
}

#include "Pulsar/Integration.h"

// collect MJD of each subint of each file into one vector
vector<MJD> pcm::get_mjds ()
{
  vector<MJD> all_mjds;
  Pulsar::Profile::no_amps = true;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load( filenames[ifile] );
    for (unsigned isubint=0; isubint < archive->get_nsubint(); isubint++)
    {
      all_mjds.push_back(archive->get_Integration(isubint)->get_epoch());
    }
  }
  catch (Error& error)
  {
    cerr << "pcm: get_mjds() error while handling " << filenames[ifile]
         << endl << "\t" << error.get_message() << endl;
    filenames.erase( filenames.begin() + ifile );
    ifile --;
  }

  Pulsar::Profile::no_amps = false;
  return all_mjds;
}

void flat (ostream& output, const Jones<double>& J)
{
  output << J.j00.real() << " " << J.j00.imag() << " "
         << J.j01.real() << " " << J.j01.imag() << " "
         << J.j10.real() << " " << J.j10.imag() << " "
         << J.j11.real() << " " << J.j11.imag();
}

void pcm::print_time_variation (SystemCalibrator* model)
{
  string filename = "temporal_variation.txt";

  ofstream output (filename.c_str());

  cerr << "pcm: printing temporal variation to " << filename << endl;

  // get all the MJDs
  vector<MJD> mjds = get_mjds();

  unsigned n_mjd = mjds.size();  // get number of MJDs in file/files used

  if (verbose)
  {
    cerr << "pcm: temporal variations printed for " << n_mjd << " MJDs: " << endl;
    for (unsigned i_mjd = 0; i_mjd < n_mjd; i_mjd++)
      cerr << "MJD[" << i_mjd << "] = " << mjds[i_mjd] << endl;
  }

  unsigned nchan = model->get_nchan();

  const Integration* subint = model->get_Archive()->get_Integration(0);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    Calibration::SignalPath* path
        = const_cast<Calibration::SignalPath*>( model->get_model(ichan) );

    if (!path->get_valid())
      continue;

    double cfreq_in_Hz = subint->get_centre_frequency(ichan) * 1e6;

    path->engage_time_variations();

    // Calibration::ReceptionModel* equation = path->get_equation();
    // path->set_transformation_index (ipath);
    // path->set_input_index (isource);

    //! Get the instrumental transformation
    const MEAL::Complex2* instrument = path->get_transformation ();

    for (unsigned i_mjd = 0; i_mjd < n_mjd; i_mjd++) try
    {
      path-> time.set_value (mjds[i_mjd]);

      Jones<double> J = instrument->evaluate();
      output << mjds[i_mjd].printdays(10) << " " << setprecision(10) << cfreq_in_Hz << " ";
      flat (output, J);
      output << endl;
    }
    catch (Error& err)
    {
    }
  }
}


#if HAVE_PGPLOT

void plot_state (SystemCalibrator* model, const string& state) try
{
  using namespace Pulsar;

  SystemCalibratorPlotter plotter (model);
  plotter.use_colour = !publication_plots;
  plotter.npanel = 4;

  //
  // if the SystemCalibrator is a ReceptionCalibrator (MEM mode)
  // and one or more flux calibrators has been included in the fit,
  // then plot the flux calibrator information as well
  //
  Reference::To<ReceptionCalibratorPlotter> rplotter = 0;
  ReceptionCalibrator* rmodel = dynamic_cast<ReceptionCalibrator*> (model);
  if (rmodel && rmodel->has_fluxcal())
  {
    rplotter = new ReceptionCalibratorPlotter (rmodel);
    rplotter->use_colour = !publication_plots;
    rplotter->npanel = 4;
  }

  //
  //
  //

  cpgbeg (0, (state + "_response.ps/CPS").c_str(), 0, 0);
  cpgsvp (.1,.9, .1,.9);

  cerr << "pcm: plotting " + state + " of instrumental response" << endl;
  plotter.plot (model);

  cpgend();

  //
  //
  //

  cpgbeg (0, (state + "_cal.ps/CPS").c_str(), 0, 0);
  cpgsvp (0.1,.9, 0.1,.9);

  cerr << "pcm: plotting " + state + " of CAL" << endl;
  plotter.plot_cal();

  cpgend();

  //
  //
  //

  if (rplotter)
  {
    cpgbeg (0, (state + "_fluxcal.ps/CPS").c_str(), 0, 0);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting " + state + " of flux calibrator" << endl;
    rplotter->plot_fluxcal();

    cpgend();
  }

  //
  //
  //

  cpgbeg (0, (state + "_psr.ps/CPS").c_str(), 0, 0);
  cpgsvp (0.1,.9, 0.1,.9);

  cerr << "pcm: plotting " + state + " pulsar states" << endl;
  plot_pulsar (plotter, *model);

  cpgend ();
}
 catch (Error& error)
  {
    cerr << "pcm: error while producing plots ignored" << endl;
  }

#endif // HAVE_PGPLOT
