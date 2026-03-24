/***************************************************************************
 *
 *   Copyright (C) 2003-2025 by Willem van Straten
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
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/RobustStepFinder.h"

#include "Pulsar/VariableTransformationFile.h"
#include "Pulsar/ManualPolnCalibrator.h"
#include "Pulsar/ConfigurableProjection.h"

#include "Pulsar/SystemCalibratorManager.h"
#include "Pulsar/SystemCalibratorUnloader.h"

#include "Pulsar/Database.h"
#include "Pulsar/StandardPrepare.h"
#include "Pulsar/DataSetManager.h"

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
  void preprocess (Pulsar::Archive*);

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

  //! Load projection configuration from filename
  void set_configurable_projection (const string& filename);

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

  //! Add to the list of files from which phase bins will be chosen
  void add_binfile (const string& filename);

  //! Add to the list of files from which templates will be loaded
  void add_template (const string& filename);

  //! Set the range of pulse phase to use as constraints
  void set_phase_range (const string& text);

  //! Assume that the specified parameter is not degenerate
  void assume_not_degenerate (const string& text);

  //! Disable the output of various plots
  void disable_plotting ();
  
  // Construct a calibrator model for MEM mode
  SystemCalibrator* measurement_equation_modeling (const string& binfile);

  // Construct a calibrator model for METM mode
  SystemCalibrator* matrix_template_matching (const string& stdname);

  void do_reparallactify (Archive* archive);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  void set_alignment_threshold (const string& arg);
  void load_calibrator_database ();
  void get_span ();
  vector<MJD> get_mjds ();
  void print_time_variation (SystemCalibrator* model);

  bool use_baseline = false;

  Reference::To<Pulsar::SystemCalibratorManager> model_manager;
  Reference::To<Pulsar::DataSetManager> data_manager;
};

Reference::To<Pulsar::StandardOptions> standard_options;

pcm::pcm () : Pulsar::Application ("pcm", 
				   "polarimetric calibration modelling")
{
  has_manual = true;
  sort_filenames = true;

  add( standard_options = new Pulsar::StandardOptions );
}

// Plot the various components of the model
void plot_state (SystemCalibrator* model, const string& state);

// Print the variations of the Jones matrices
void print_time_variation (SystemCalibrator* model);

static bool plot_chosen_bins = true; 
static bool plot_onpulse_bins = true; 

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

void plot_onpulse (Pulsar::ReceptionCalibrator& model, Pulsar::Archive* archive, string base = "onpulse")
{
  string device = base + ".ps/cps";
  cpgbeg (0, device.c_str(), 0, 0);

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

// name(s) of file(s) from which phase bins will be chosen
vector<string> binfiles;

string append (string before, string between, string after, bool insert)
{
  if (insert)
    return before + "_" + between + after;
  else
    return before + after;
}

void auto_select (Pulsar::ReceptionCalibrator& model,
                  Pulsar::Archive* archive,
                  unsigned maxbins)
{
  cerr << "pcm: choosing up to " << maxbins << " pulse phase bins" << endl;
  vector<unsigned> bins;

  if (!prepare)
    throw Error (InvalidState, "auto_select", "StandardPrepare policy not set");

  archive->fscrunch ();
  archive->tscrunch ();
  archive->remove_baseline();
  prepare->prepare (archive);
      
  prepare->set_input_states (maxbins);
  prepare->choose (archive);
  prepare->get_bins (bins);

  sort (bins.begin(), bins.end());

  for (unsigned ibin=0; ibin < bins.size(); ibin++)
  {
    // cerr << "pcm: adding phase bin " << bins[ibin] << endl;
    model.add_state (bins[ibin]);
  }

  model.set_standard_data( archive );

  string name = archive->get_source ();

#if HAVE_PGPLOT
  if (plot_chosen_bins)
  {
    string dev = append ("chosen", name, "", binfiles.size() > 1);
    plot_chosen (archive, bins, dev);
  }
  
  if (plot_onpulse_bins)
  {
    string dev = append ("onpulse", name, "", binfiles.size() > 1);
    plot_onpulse (model, archive, dev);
  }
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
Reference::To<Pulsar::Calibrator::Type> model_type = new Pulsar::CalibratorTypes::van04_Eq18;

// unloads the solution(s)
Pulsar::SystemCalibrator::Unloader unloader;

// verbosity flags
// bool verbose = false;

// The maximum number of bins to use
unsigned maxbins = 16;

// The pulse phase window to use
float phmin = 0, phmax = 0;

// The phase bins to add to the model
vector<unsigned> phase_bins;

// Flag raised when software may choose the maximum harmonic
bool choose_maximum_harmonic = false;

// METM Mode: Solve the measurement equation for each observation
bool solve_each = false;

// METM Mode: Share a single phase estimate between all observations
bool shared_phase = false;

// significance of phase shift required to fail test
float alignment_threshold = 4.0; // sigma

// significance of phase shift required to automatically rotate in phase
float auto_alignment_threshold = 0.0; // sigma

// StandardPrepare::prepare calls Archive::centre to ensure that input data are aligned in phase
bool align_phase = true;

// total intensity profile of first archive, used to check for phase jumps
Reference::To<DataSetManager> phase_std_manager;

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
bool normalize_calibrated_by_invariant = false;
bool independent_gains = false;
bool step_after_cal = false;
bool refcal_through_frontend = true;
bool physical_coherency = false;

float retry_chisq = 0.0;
float invalid_chisq = 0.0;

// filename of previous pcm solution to be used as first guess
string previous_solution_filename;

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

Reference::To< Pulsar::VariableTransformationManager > projection;
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
  throw Error (InvalidParam, "add_step",
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
static bool input_data_report = false;
static bool data_and_model_report = false;
static bool total_invariant_report = false;
static bool covariance_report = false;

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
    input_data_report = true;

  else if (name == "data")
    data_and_model_report = true;

  else if (name == "chisq" || name == "report")
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

  else if (name == "failed")
    failed_report = true;

  else if (name == "covariance")
  {
    cerr << "pcm: will print covariance report" << endl;
    covariance_report = true;
  }
  else if (name == "invint")
  {
    cerr << "pcm: will print the normalization factor applied to each profile (based on total invariant)" << endl;
    total_invariant_report = true;
  }
  else if (name == "temporal")
  {
    cerr << "pcm: will print temporal variations" << endl;
    print_variation = true;
  }
  else
  {
    cerr << "pcm: unrecognized diagnostic name '" << name << "'" << endl;
    exit (-1);
  }
}

// names of files containing a Calibration Database
vector<string> cal_dbase_filenames;

// name of file containing the calibrated template
vector<string> template_filenames;

// throw an exception when no polncal observation are available
bool must_have_cals = true;

// look for PolnCal observations with nearby sky coordinates
bool check_coordinates = true;

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

// Number of threads used to solve equations
unsigned nthread = 1;

// name of file containing list of filenames to be calibrated
string calibrate_these;

// name of least squares minimization algorithm
string least_squares;

// name of file containing MEAL::Function text interface commands
vector<string> equation_configuration;

bool unload_each_calibrated = true;
bool fscrunch_data_to_model = false;

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

void pcm::set_projection (const string& filename) try
{
  cerr << "pcm: loading known projections from " << filename << endl;
  auto cal = new ManualPolnCalibrator (filename);
  auto known = new VariableTransformationFile (cal);

  if (!projection)
  {
    projection = known;
    return;
  }

  auto confable = dynamic_cast<ConfigurableProjection*> (projection.get());
  if (confable)
  {
    cerr << "pcm: setting the known projection of the configurable projection" << endl;
    confable->set_projection(known);
  }
}
catch (Error& error)
{
  cerr << "pcm: failed to load known projections from " << filename << endl;
  throw error;
}

void pcm::set_configurable_projection (const string& filename) try
{
  cerr << "pcm: loading configurable projection from " << filename << endl;
  auto cal = new ConfigurableProjection (filename);

  if (projection)
  {
    auto known = dynamic_cast<KnownVariableTransformation*> (projection.get());

    if (known)
    {
      cerr << "pcm: setting the known projection of the configurable projection" << endl;
      cal->set_projection(known);
    }
  }

  projection = cal;
}
catch (Error& error)
{
  cerr << "pcm: failed to load configurable projection from " << filename << endl;
  throw error;
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

  cerr << "pcm: using a polynomial of order " << order << " to model ";
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

void pcm::add_binfile (const string& filename)
{
  cerr << "pcm: adding " << filename << " to phase bin reference files" << endl;
  binfiles.push_back (filename);
}

void pcm::add_template (const string& filename)
{
  cerr << "pcm: adding " << filename << " to template files" << endl;
  template_filenames.push_back (filename);
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

void pcm::disable_plotting ()
{
  plot_onpulse_bins = false;
  plot_chosen_bins = false;
}

//! Add command line options
void pcm::add_options (CommandLine::Menu& menu)
{
  if (!data_manager)
    data_manager = new DataSetManager;
    
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

  arg = menu.add (&unloader, &SystemCalibrator::Unloader::set_archive_class, 'A', "class");
  arg->set_help ("set the output archive class name");

  arg = menu.add (unload_path, 'O', "path");
  arg->set_help ("set directory to which outputs are written");

  arg = menu.add (output_filename, "out", "fname");
  arg->set_help ("set cal solution output filename "
		 "(default=" + output_filename + ")");

  arg = menu.add (unload_each_calibrated, 'N');
  arg->set_help ("do not unload calibrated data files");

  arg = menu.add (normalize_calibrated_by_invariant, "normalize");
  arg->set_help ("normalize output Stokes parameters by total invariant interval");

  arg = menu.add (this, &pcm::disable_plotting, "noplots");
  arg->set_help ("do not plot chosen.ps and onpulse.ps");

  menu.add ("\n" "Input options:");

  arg = menu.add (calfile, 'C', "file");
  arg->set_help ("filename with list of calibrator files");

  arg = menu.add (this, &pcm::add_calibrator_database, 'd', "file");
  arg->set_help ("add file to list of calibrator databases");

  arg = menu.add (calibrate_these, 'W', "file");
  arg->set_help ("filename with list of other data files to be calibrated");

  arg = menu.add (data_manager.get(), &DataSetManager::set_fluxcal_days,
		  'F', "days");
  arg->set_help ("use flux calibrators within days of pulsar data mid-time");

  arg = menu.add (data_manager.get(), &DataSetManager::set_polncal_hours,
		  'L', "hours");
  arg->set_help ("use reference sources within hours of pulsar data mid-time");

  arg = menu.add (must_have_cals, 'w');
  arg->set_help ("continue if no calibrators are found");

  arg = menu.add (ProjectionCorrection::trust_pointing_feed_angle, "fa");
  arg->set_help ("always trust the pointing:fa (feed angle) attribute");
  menu.add( new CommandLine::Alias( arg, 'y' ) );

  arg = menu.add (ProjectionCorrection::trust_pointing_para_angle, "va");
  arg->set_help ("always trust the pointing:va (vertical/parallactic angle) attribute");

  arg = menu.add (check_coordinates, 'Z');
  arg->set_help ("ignore the sky coordinates of PolnCal observations");

  arg = menu.add (previous_solution_filename, "solution", "file");
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

  arg = menu.add (this, &pcm::set_configurable_projection, "projection", "file");
  arg->set_help ("load projection model configuration from YAML file");

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
  arg->set_help ("enable diagnostic: name=report,guess,residual,result,total,invint");

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
  arg->set_help ("model PAR as polyomial of order N");
  arg->set_long_help (par_help);

  arg = menu.add (this, &pcm::set_selection_policy, 'B', "choose");
  arg->set_help ("set the phase bin selection policy: int, pol, orth, inv");
  arg->set_long_help ("separate multiple policies with commas");

  arg = menu.add (this, &pcm::add_binfile, 'c', "file");
  arg->set_help ("choose best input states from observation in file");

  arg = menu.add (this, &pcm::add_phase_bin, 'b', "nbin");
  arg->set_help ("add phase bin to constraints");

  arg = menu.add (maxbins, 'n', "nbin");
  arg->set_help ("set the number of phase bins to choose as input states");

  // arg = menu.add (this, &pcm::set_phase_range, 'p', "pA,pB");
  // arg->set_help ("set the phase window from which to choose input states");

  arg = menu.add (this, &pcm::set_alignment_threshold, 'a', "bins");
  arg->set_help ("set the threshold for testing input data phase alignment");

  arg = menu.add (align_phase, "noalign");
  arg->set_help ("disable phase alignment using Archive::centre");

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

  arg = menu.add (use_baseline, 'z');
  arg->set_help ("assume that the off-pulse baseline has zero Stokes V");

  arg = menu.add (model_fluxcal_on_minus_off, 'Y');
  arg->set_help ("model the difference between FluxCal-On and FluxCal-Off");

  arg = menu.add (multiple_flux_calibrators, 'f');
  arg->set_help ("model each FluxCal with unique Stokes parameters");

  arg = menu.add (this, &pcm::assume_not_degenerate, 'T', "code");
  arg->set_help ("assume Stokes V [b]oost and/or [r]otation is not degenerate");
  
  menu.add ("\n"
	    "METM: Measurement Equation Template Matching\n"
	    "  -- observations of a known source as in van Straten (2013) \n");

  arg = menu.add (this, &pcm::add_template, 'S', "file");
  arg->set_help ("add filename of calibrated standard");

  arg = menu.add (fscrunch_data_to_model, 'G');
  arg->set_help ("fscrunch data to match number of channels of standard");

  arg = menu.add (choose_maximum_harmonic, 'H');
  arg->set_help ("automatically choose the number of harmonics");

  arg = menu.add (maxbins, 'n', "nharm");
  arg->set_help ("set the number of harmonics to use as input states");

  arg = menu.add (solve_each, '1');
  arg->set_help ("solve independently for each observation");
}

Reference::To<Pulsar::PolnCalibrator> previous_solution;
Reference::To<Pulsar::CalibratorStokes> previous_cal;

void pcm::setup ()
{
  if (nthread == 0)
    throw Error (InvalidState, "pcm",
		 "invalid number of threads = %u", nthread);

  if (! choose_maximum_harmonic)
    cerr << "pcm: using a maximum of " << maxbins << " bins or harmonics" << endl;

  bool mem_mode = template_filenames.empty();
  
  if (mem_mode && phmin == phmax && binfiles.empty())
    throw Error (InvalidState, "pcm",
      "In MEM mode, at least one of the following options"
      " must be specified:\n"
      " -p min,max  Choose constraints from the specified pulse phase range \n"
      " -c archive  Choose optimal constraints from the specified archive");

  if (!previous_solution_filename.empty())
  {
    Reference::To<Archive> cal = Archive::load (previous_solution_filename);
    previous_solution = new PolnCalibrator (cal);
    previous_cal = cal->get<CalibratorStokes>();

    const Calibrator::Type* type = previous_solution->get_type ();

    cerr << "pcm: previous solution has type=" << type->get_name()
         << " and nparam=" << type->get_nparam() << endl;
    
    if (!type->is_a (model_type) || !model_type->is_a (type))
    {
      cerr << "pcm: over-riding model type=" << model_type->get_name() << endl;
      model_type = type->clone();
    }
  }
  
  if (mem_mode && fscrunch_data_to_model)
    throw Error (InvalidState, "pcm",
		 "In MEM mode, the -G option is not supported");

  if (!mem_mode)
    alignment_threshold = 0.0;

  if (alignment_threshold || auto_alignment_threshold)
    phase_std_manager = new DataSetManager;

  load_calibrator_database();

  if (!prepare)
  {
    Calibration::MultipleRanking* mult = new Calibration::MultipleRanking;
    mult->add( new Calibration::MaximumPolarization );
    mult->add( new Calibration::MaximumIntensity );
    prepare = mult;
  }

  if (normalize_calibrated_by_invariant || !align_phase)
  {
    /*
    By default, StandardPrepare::prepare calls Archive::centre(0.0) to ensure that all
    input data are aligned in phase.  The resulting phase shift is confusing
    if it is also applied to the calibrated output; therefore, by default, phase alignment
    is disabled before using StandardPrepare::prepare to produce calibrated output.

    However, this causes trouble when the same "on pulse" window that is used to compute
    the invariant (the square root of the total squared invariant, integrated over all onpulse
    phase bins) of the input data is also used to compute the invariant of the output data.

    Therefore, when normalizing the calibrated output data is enabled, phase alignment is disabled.
    */
    cerr << "pcm: disabling phase alignment so that internal phase = output phase" << endl;
    prepare->set_align_phase(false);
  }

  unloader.set_program ( "pcm" );
  unloader.set_filename( output_filename );
}

vector< Reference::To<Pulsar::Archive> > total;
Reference::To<Pulsar::Archive> archive;

#include "Pulsar/VariableFaradayRotation.h"

void configure_model (Pulsar::SystemCalibrator* model)
{
  model->set_nthread (nthread);
  model->set_report_projection (true);

  if (ionospheric_rm)
  {
    auto rot = new VariableFaradayRotation;
    rot->set_ionospheric_rotation_measure (ionospheric_rm);
    model->set_faraday_rotation (rot);
  }

  if (previous_solution)
    model->set_previous_solution (previous_solution);
  
  if (previous_cal)
    model->set_previous_cal (previous_cal);

  model->set_cal_outlier_threshold (cal_outlier_threshold);
  model->set_cal_intensity_threshold (cal_intensity_threshold);
  model->set_cal_polarization_threshold (cal_polarization_threshold);

  if (step_threshold)
    model->set_step_finder( new RobustStepFinder (step_threshold) );

  model->set_report_initial_state (prefit_report);
  model->set_report_input_data (input_data_report);
  model->set_report_input_failed (failed_report);
  model->set_report_data_and_model (data_and_model_report);
  model->set_report_total_invariant (total_invariant_report);
  model->set_report_covariance (covariance_report);

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
  if (!phase_std_manager)
    throw Error (InvalidState, "check_phase", "no phase standard manager");
  
  Reference::To<Pulsar::Archive> temp = archive->total();

  DataSet* dataset = phase_std_manager->get (archive);
  Archive* total = dataset->get_total();

  /*
    test for phase shift only if phase_std is not from current archive.
    this test will fail if binfile is a symbollic link.
  */

  if (archive->get_filename() == total->get_filename())
    return;
  
  Profile* phase_std = total->get_Profile(0,0,0);
  
  Estimate<double> shift = phase_std->shift (temp->get_Profile(0,0,0));

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

  if ( rcvr->get_projection_corrected () )
  {
    cerr << "pcm: re-parallactifying data" << endl;
    ProjectionCorrection projection;

    rcvr->set_projection_corrected (false);
    projection.set_archive( archive );

    unsigned nsub = archive->get_nsubint();
    for (unsigned isub=0; isub < nsub; isub++)
    {
      Pulsar::Integration* subint = archive->get_Integration (isub);
 
      // the returned matrix transforms from the corrected to the observed
      Jones<double> xform = projection (isub);
      subint->expert()->transform (xform);
    }
  }
}

void pcm::preprocess (Pulsar::Archive* archive)
{
  if (archive->get_type() == Signal::Pulsar)
  {
    if (verbose)
      cerr << "pcm: preparing pulsar data" << endl;

    prepare->prepare (archive);

    if (reparallactify)
      do_reparallactify (archive);
  }
}

void pcm::process (Pulsar::Archive* archive)
{
  preprocess (archive);

  if (!model_manager)
  {
    cerr << "pcm: creating model manager" << endl;

    model_manager = new SystemCalibratorManager;

    model_manager->set_fscrunch_data_to_model (fscrunch_data_to_model);
      
    for (auto filename: template_filenames)
    {
      cerr << "pcm: constructing METM with " << filename << endl;
      SystemCalibrator* model = matrix_template_matching (filename);
      configure_model( model );  
      model_manager->manage( model );
    }

    for (auto filename: binfiles)
    {
      cerr << "pcm: constructing MEM with " << filename << endl;
      SystemCalibrator* model = measurement_equation_modeling (filename);
      configure_model( model );  
      model_manager->manage( model );
    }
  }

  if (phase_std_manager)
  {
    if (verbose)
      cerr << "pcm: checking phase" << endl;

    check_phase (archive);

    DataSet* dataset = phase_std_manager->get (archive);

    if (!dataset)
    {
      cerr << "pcm: creating phase reference" << endl;

      // store an fscrunched and tscrunched clone for phase reference
      phase_std_manager->integrate (archive->total());
    }
  }

  cerr << "pcm: adding observation file=" << archive->get_filename() << endl;

  model_manager->preprocess( archive );
  model_manager->add_observation( archive );
}

void pcm::finalize ()
{

#if 0 
  // TO-DO TODO FIX
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
#endif

#if HAVE_PGPLOT

  try {

    if (plot_guess)
    {
      SystemCalibrator* model = model_manager->get_model ();
  
      Pulsar::SystemCalibratorPlotter plotter (model);
      plotter.use_colour = !publication_plots;

      plot_state (model, "guess");
    }

    if (plot_residual)
    {
      for (unsigned ical=0; ical < model_manager->get_ncalibrator(); ical++)
      {
	SystemCalibrator* model = model_manager->get_calibrator (ical);

	if ( model->get_nstate_pulsar() )
	{
	  Pulsar::SystemCalibratorPlotter plotter (model);
	  plotter.use_colour = !publication_plots;

	  cerr << "pcm: plotting pulsar constraints" << endl;
	  plot_constraints (plotter, model->get_nchan());
	}
      }
    }
  }
  catch (Error& error)
  {
    cerr << "pcm: error while producing plots ignored" << endl;
  }

#endif // HAVE_PGPLOT

  try
  {
    cerr << "pcm: solving model" << endl;
    model_manager->solve ();
  }
  catch (Error& error)
  {
    cerr << error << endl;
    return;
  }

  SystemCalibrator* model = model_manager->get_model ();
    
  if (model->has_valid())
  {
    cerr << "pcm: unload model" << endl;
    unloader.unload (model);
  }
  else
  {
    cerr << "pcm: no valid solutions to unload" << endl;
    return;
  }

  if (print_variation && get_time_variation())
  {
    cerr << "pcm: print variation" << endl;
    print_time_variation (model);
  }

#if HAVE_PGPLOT

  Pulsar::SystemCalibratorPlotter plotter (model);
  plotter.use_colour = !publication_plots;
      
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
#if 0
  else
  {
    for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
      dirglob (&filenames, calibrator_filenames[ical]);

    cerr << "pcm: calibrating archives (PSR and CAL)" << endl;
  }
#endif

  DataSetManager total_manager;

  // disable phase alignment (prepare->prepare is called during pcm::preprocess)
  // It's confusing when *.calib are output with pulsar phase offset from the input data
  prepare->set_align_phase(false);

  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    if (verbose)
      cerr << "pcm: loading " << filenames[i] << endl;

    archive = Pulsar::Archive::load(filenames[i]);

    cout << "pcm: loaded archive: " << filenames[i] << endl;

    standard_options->process (archive);
    preprocess (archive);
    model_manager->precalibrate (archive);

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
        cerr << "pcm: add to calibrated total" << endl;

      total_manager.integrate (archive);
    }
  }
  catch (Error& error)
  {
    cerr << error << endl;
  }

  if (total_manager.get_integration_length())
  {
    unsigned ntotal = total_manager.get_nset ();
    for (unsigned itotal=0; itotal < ntotal; itotal++) try
    {
      DataSet* dataset = total_manager.get_set (itotal);
      
      string name = dataset->get_name();
      Archive* total = dataset->get_total();

      cerr << "pcm: writing total integrated result for " << name << endl;

      string filename = append ("total", name, ".ar", ntotal > 1);
      total->unload (filename);

#if HAVE_PGPLOT

      if (plot_total)
      {
        string dev = append ("calibrated", name, ".ps/CPS", ntotal > 1);

        cpgbeg (0, dev.c_str(), 0, 0);
        cpgask(1);
        cpgslw(2);
        cpgsvp (.1,.9, .1,.9);

        total->fscrunch();
        total->remove_baseline();

        cerr << "pcm: plotting calibrated result for " << name << endl;
        Pulsar::StokesSpherical plot;
        plot.plot (total);

        cpgend ();
      }

      if (phase_bins.size() != 0)
      {
        total->fscrunch ();
        total->tscrunch ();
        prepare->prepare (total);

        if (plot_chosen_bins)
        {
          string dev = append ("selected", name, "", ntotal > 1);
          plot_chosen (total, phase_bins, dev);
        }
      }
#endif // HAVE_PGPLOT
    }
    catch (Error& error)
    {
      cerr << error << endl;
    }
  }

  cerr << "pcm: finished" << endl;
}


using namespace Pulsar;

SystemCalibrator* pcm::measurement_equation_modeling (const string& binfile) try
{
  ReceptionCalibrator* model = new ReceptionCalibrator (model_type);

  model->output_report = output_report;

  if (use_baseline)
  {
    cerr << "pcm: assuming that off-pulse baseline has zero Stokes V" << endl;
    degenerate_V_boost = false;
  }

  model->degenerate_V_boost = degenerate_V_boost;
  model->measure_cal_V = measure_cal_V;

  if (!degenerate_V_boost)
  {
    cerr << "pcm: boost along Stokes V is not degenerate" << endl;
    cerr << "pcm: allowing CAL Stokes V to vary" << endl;
    model->measure_cal_V = true;
  }
  else
  {
    cerr << "pcm: boost along Stokes V is intrinsically degenerate" << endl;
    if (measure_cal_V)
      cerr << "pcm: if available, fluxcal data will be used to constrain CAL Stokes V" << endl;
    else
      cerr << "pcm: assuming that CAL Stokes V = 0" << endl;
  }

  model->degenerate_V_rotation = degenerate_V_rotation;
  model->measure_cal_Q = measure_cal_Q;

  if (!degenerate_V_rotation)
  {
    cerr << "pcm: rotation about Stokes V is not degenerate" << endl;
    cerr << "pcm: allowing CAL Stokes Q to vary" << endl;
    model->measure_cal_Q = true;
  }
  else
  {
    cerr << "pcm: rotation about Stokes V is intrinsically degenerate" << endl;
    if (measure_cal_Q)
      cerr << "pcm: allowing CAL Stokes Q to vary" << endl;
    else
      cerr << "pcm: assuming that CAL Stokes Q = 0" << endl;
  }

  if (equal_ellipticities)
    cerr << "pcm: assuming that the receptor ellipticities are equal" << endl;

  model->equal_ellipticities = equal_ellipticities;

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model->set_normalize_by_invariant( normalize_by_invariant );

  if (normalize_calibrated_by_invariant)
  {
    cerr << "pcm: normalizing output calibrated Stokes parameters by invariant" << endl;
  }

  model->set_normalize_calibrated_by_invariant( normalize_calibrated_by_invariant );

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
  
  cerr << "pcm: selecting phase bins from " << binfile << endl;
  Reference::To<Pulsar::Archive> autobin = load (binfile);
  auto_select (*model, autobin, maxbins);

  if (phase_std_manager)
    phase_std_manager->integrate( autobin );
    
  model->set_name (autobin->get_source ());

  if (!autobin)
    throw Error (InvalidState, "pcm::measurement_equation_modeling",
		 "need to update MEM code to work without -c option");
  
  cerr << "pcm: get calibrator filenames" << endl;

  try
  {
    DataSet* dataset = data_manager->get (autobin);
    vector<string> filenames = dataset->get_calibrator_filenames ();

    if (!calfile.empty())
    {
      cerr << "pcm: loading calibrator filenames from " << calfile << endl;
      stringfload (&filenames, calfile);
    }

    cerr << "pcm: set calibrators" << endl;
    model->set_calibrator_preprocessor (standard_options);
    model->set_calibrators (filenames);
  }
  catch (Error& error)
  {
    cerr << "pcm: error (ignored) loading calibrator filenames - " << error.get_message() << endl;
  }

  // add the specified phase bins
  for (unsigned ibin=0; ibin<phase_bins.size(); ibin++)
    model->add_state (phase_bins[ibin]);

  if (use_baseline)
  {
    model->add_state (Calibration::SourceEstimate::baseline_mean);
  }

  cerr << "pcm: " << model->get_nstate_pulsar() << " states" << endl;
  if ( model->get_nstate_pulsar() == 0 )
    throw Error (InvalidState, "pcm::measurement_equation_modeling",
                 "no pulsar phase bins have been selected");

  return model;
}
catch (Error& error)
{
  throw error += "pcm::measurement_equation_modeling";
}

SystemCalibrator* pcm::matrix_template_matching (const string& stdname)
{
  PulsarCalibrator* model = new PulsarCalibrator (model_type);

  if (shared_phase)
    model->share_phase ();

  if (choose_maximum_harmonic)
  {
    cerr << "pcm: choosing the maximum harmonic" << endl;
    model->set_choose_maximum_harmonic ();
  }
  else
  {
    cerr << "pcm: using " << maxbins << " harmonics" << endl;
    model->set_maximum_harmonic (maxbins);
  }

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

  try
  {
    DataSet* dataset = data_manager->get (standard);

    const vector<string>& filenames = dataset->get_calibrator_filenames ();
  
    if (filenames.size())
      cerr << "pcm: adding " << filenames.size() << " calibrators" << endl;

    for (unsigned ical=0; ical < filenames.size(); ical++)
    {
      Reference::To<Archive> cal = Archive::load (filenames[ical]);
      standard_options->process (cal);

      model->add_observation( cal );
    }
  }
  catch (Error& error)
  {
    cerr << "pcm: error (ignored) loading calibrator filenames - " << error.get_message() << endl;
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
    span *= 24;
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

  if (!data_manager)
    data_manager = new DataSetManager;

  data_manager -> load (filenames);
  
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

  data_manager -> set_database( database );
  data_manager -> set_check_coordinates ( check_coordinates );
  data_manager -> find_poln_calibrators ();
  
  if (data_manager->get_polncal_count () == 0)
  {
    cerr << "pcm: no PolnCal observations found" << endl;

    if (must_have_cals)
    {
      cerr << "pcm: cannot continue (disable this check with -w)" << endl;
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

  if (!template_filenames.empty())
  {
    cerr << "pcm: no need for flux calibrator observations" << endl;
    return;
  }
    
  data_manager -> find_on_flux_calibrators ();

  if (model_fluxcal_on_minus_off)
    data_manager -> find_off_flux_calibrators ();
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
    auto path = const_cast<Calibration::SignalPath*>( model->get_model(ichan) );

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
