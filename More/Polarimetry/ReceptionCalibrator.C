#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

#include "Calibration/Gain.h"
#include "Calibration/Boost.h"
#include "Calibration/SingleAxis.h"

#include <algorithm>
#include <assert.h>

#if defined(__GNUC__) && (__GNUC__ < 3)
#define WORK_AROUND_COMPILER
#endif

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
Pulsar::ReceptionCalibrator::ReceptionCalibrator (const Archive* archive)
{
  is_fit = false;
  is_initialized = false;

  PA_min = PA_max = 0.0;

  if (archive)
    initial_observation (archive);

  time.connect (&parallactic, &Calibration::Parallactic::set_epoch);

}

void Pulsar::ReceptionCalibrator::set_calibrators (const vector<string>& names)
{
  calibrator_filenames = names;
}



Pulsar::StandardModel::StandardModel (Model _model)
{
  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the model of the instrument
  //

  model = _model;

  switch (model) {
  case Hamaker:
    polar = new Calibration::PolarEstimate;
    instrument = polar;
    break;
  case Britton:
    physical = new Calibration::InstrumentEstimate;
    instrument = physical;
    break;
  }

  equation = new Calibration::ReceptionModel;

  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the signal path seen by the calibrator
  //
  
  // allow the differential gain and phase of the backend to vary
  // between pulsar and calibrator observations

  backend = new Calibration::SingleAxis;

  // disable fit for absolute Gain
  backend->set_infit (0, false);
  
  pcal_path = new Calibration::ProductTransformation;
  pcal_path->add_Transformation( backend );
  pcal_path->add_Transformation( instrument );
  
  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the signal path seen by the pulsar
  //

  pulsar_path = new Calibration::ProductTransformation;
  pulsar_path->add_Transformation( instrument );

}


void Pulsar::StandardModel::update ()
{
  switch (model) {
  case Hamaker:
    polar -> update ();
    break;
  case Britton:
    physical -> update();
    break;
  }
}

void Pulsar::ReceptionCalibrator::initial_observation (const Archive* data)
{
  if (!data)
    throw Error (InvalidState, "ReceptionCalibrator::initial_observation",
		 "no Archive");

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::initial_observation" << endl;

  if (data->get_type() != Signal::Pulsar)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() 
		 + "' not a Pulsar observation");

  assert_full_poln (data, "Pulsar::ReceptionCalibrator::initial_observation");

  if (data->get_parallactic_corrected ())
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has been corrected for parallactic angle rotation");
		 
  if (!data->get_dedispersed ())
    cerr << "Pulsar::ReceptionCalibrator WARNING archive not dedispersed\n"
      "  Pulse phase will vary as a function of frequency channel" << endl;

  uncalibrated = data->clone();

  parallactic.set_source_coordinates( uncalibrated->get_coordinates() );

  float latitude = 0, longitude = 0;
  uncalibrated->telescope_coordinates (&latitude, &longitude);
  parallactic.set_observatory_coordinates (latitude, longitude);

  unsigned nchan = uncalibrated->get_nchan();

  model.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    model[ichan] = new StandardModel;

    model[ichan]->pulsar_path->add_Transformation( &parallactic );

  }

  if (calibrator.source.size() == 0 && calibrator_filenames.size())
    load_calibrators ();

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    init_estimate ( pulsar[istate] );

  start_epoch = end_epoch = data->start_time ();

  parallactic.set_epoch (start_epoch);
  PA_max = PA_min = parallactic.get_param (0);

}

void Pulsar::ReceptionCalibrator::load_calibrators ()
{

  for (unsigned ifile = 0; ifile < calibrator_filenames.size(); ifile++) {
    
    try {

      if (verbose)
	cerr << "Pulsar::ReceptionCalibrator::load_calibrators loading "
	     << calibrator_filenames[ifile] << endl;

      Reference::To<Archive> archive;
      archive = Pulsar::Archive::load(calibrator_filenames[ifile]);

      add_calibrator (archive);

    }
    catch (Error& error) {
      cerr << "Pulsar::ReceptionCalibrator::load_calibrators ERROR" 
	   << error << endl;
    }

  }

  unsigned nchan = model.size();

  cerr << "Setting " << nchan << " channel receiver" << endl;
  for (unsigned ichan=0; ichan<nchan; ichan+=1)
    model[ichan]->update ();

}


//! Add the specified pulse phase bin to the set of state constraints
void Pulsar::ReceptionCalibrator::add_state (unsigned phase_bin)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_state", false);

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" 
	 << phase_bin << endl;

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    if (pulsar[istate].phase_bin == phase_bin) {
      cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << phase_bin
	   << " already in use" << endl;
      return;
    }

  pulsar.push_back( SourceEstimate (phase_bin) );

  if (uncalibrated)
    init_estimate( pulsar.back() );
}

void Pulsar::ReceptionCalibrator::init_estimate (SourceEstimate& estimate)
{
  unsigned nchan = uncalibrated->get_nchan ();
  unsigned nbin = uncalibrated->get_nbin ();

  if (estimate.phase_bin >= nbin)
    throw Error (InvalidRange, "Pulsar::ReceptionCalibrator::init_estimate",
		 "phase bin=%d >= nbin=%d", estimate.phase_bin, nbin);

  estimate.source.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    unsigned nsource = model[ichan]->equation->get_nsource();
    if (ichan==0)
      estimate.source_index = nsource;
    else if (estimate.source_index != nsource)
      throw Error (InvalidState, "Pulsar::ReceptionCalibrator::init_estimate",
		   "isource=%d != nsource=%d", estimate.source_index, nsource);

    model[ichan]->equation->add_source( &(estimate.source[ichan]) );
  }

}

//! Get the number of pulsar phase bin input polarization states
unsigned Pulsar::ReceptionCalibrator::get_nstate_pulsar () const
{
  return pulsar.size();
}

//! Get the total number of input polarization states
unsigned Pulsar::ReceptionCalibrator::get_nstate () const
{
  if (model.size() == 0)
    return 0;

  return model[0]->equation->get_nsource ();
}

unsigned Pulsar::ReceptionCalibrator::get_nchan () const
{
  return model.size();
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_calibrator (const Archive* data)
{
  if (!uncalibrated)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_calibrator",
		 "No Archive containing pulsar data has yet been added");

  Reference::To<PolnCalibrator> polarcal = new PolarCalibrator (data);

  polarcal->build( uncalibrated->get_nchan() );

  add_PolnCalibrator (polarcal);

}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_observation (const Archive* data)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_observation", false);

  if (data->get_type() == Signal::PolnCal) {
    add_calibrator (data);
    return;
  }

  if (!uncalibrated)
    initial_observation (data);

  string reason;
  if (!uncalibrated->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "'" + data->get_filename() + "' does not match "
		 "'" + uncalibrated->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    if (epoch < start_epoch)
      start_epoch = epoch;
    if (epoch > end_epoch)
      end_epoch = epoch;

    parallactic.set_epoch (epoch);
    float PA = parallactic.get_param (0);

    if (PA < PA_min)
      PA_min = PA;
    if (PA > PA_max)
      PA_max = PA;

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      // the selected pulse phase bins
      Calibration::Measurements measurements ( time.new_Value(epoch) );

      for (unsigned istate=0; istate < pulsar.size(); istate++)
	add_data (measurements, pulsar[istate], ichan, integration);

      Calibration::ProductTransformation* new_path;
      new_path = model[ichan]->pulsar_path->clone();

      new_path->add_Transformation (new Calibration::Gain);

      model[ichan]->equation->add_path (new_path);
      model[ichan]->equation->add_data (measurements);

    }
  }
}

void
Pulsar::ReceptionCalibrator::add_data(vector<Calibration::MeasuredState>& bins,
				      SourceEstimate& estimate,
				      unsigned ichan,
				      const Integration* data)
{
  unsigned nchan = data->get_nchan ();

  // sanity check
  if (estimate.source.size () != nchan)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_data",
		 "SourceEstimate.nchan=%d != Integration.nchan=%d",
		 estimate.source.size(), nchan);

  unsigned ibin = estimate.phase_bin;

  Estimate<Stokes<float>, float> stokes;

  // optimization: the variance is the same for all pulse phase bins
  float* var = 0;
  if (bins.size() == 0)
    var = &(stokes.var);
  else
    stokes.var = bins[0].var;

  stokes.val = data->get_Stokes ( ichan, ibin, var );

  // NOTE: the measured states are NOT corrected for PA
  Calibration::MeasuredState state;
  state.val = stokes.val;
  state.var = stokes.var;

  bins.push_back ( state );
  bins.back().source_index = estimate.source_index;

  /* Correct the stokes parameters using the current best estimate of
     the instrument and the parallactic angle rotation before adding
     them to best estimate of the input state */

  Jones<double> correct = inv( model[ichan]->pulsar_path->evaluate() );

  stokes.val = correct * stokes.val * herm(correct);

  estimate.source[ichan].mean += stokes;

}

//! Add the specified PolnCalibrator observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_PolnCalibrator (const PolnCalibrator* p)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_PolnCalibrator");

  const Archive* cal = p->get_Archive();

  if (cal->get_state() != Signal::Coherence)
    throw Error (InvalidParam, 
		 "Pulsar::ReceptionCalibrator::add_PolnCalibrator",
		 "Archive='" + cal->get_filename() + "' "
		 "invalid state=" + State2string(cal->get_state()));

  string reason;
  if (!uncalibrated->calibrator_match (cal, reason))
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::add_PolnCalibrator",
		 "'" + cal->get_filename() + "' does not match "
		 "'" + uncalibrated->get_filename() + reason);

  unsigned nchan = uncalibrated->get_nchan ();
  unsigned nsub = cal->get_nsubint();
  unsigned npol = cal->get_npol();
  
  assert (npol == 4);

  if (calibrator.source.size() == 0) {

    // add the calibrator states to the equations
    init_estimate (calibrator);

    // set the initial guess and fit flags
    Stokes<double> cal_state (1,0,.5,0);

    for (unsigned ichan=0; ichan<nchan; ichan++) {
      
      calibrator.source[ichan].set_stokes ( cal_state );

      for (unsigned istokes=0; istokes<4; istokes++)
	calibrator.source[ichan].set_infit (istokes, false);

      // Stokes U may vary
      calibrator.source[ichan].set_infit (2, true);

    }

  }

  const PolarCalibrator* polcal = dynamic_cast<const PolarCalibrator*>(p);

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  for (unsigned isub=0; isub<nsub; isub++) {

    p->get_levels (isub, nchan, cal_hi, cal_lo);

    const Integration* integration = cal->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    if (epoch < start_epoch)
      start_epoch = epoch;
    if (epoch > end_epoch)
      end_epoch = epoch;

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      unsigned ipol = 0;

      // transpose [ipol][ichan] output of PolnCalibrator::get_levels
      vector< Estimate<double> > cal (npol);
      for (ipol = 0; ipol<npol; ipol++)
	cal[ipol] = cal_hi[ipol][ichan] - cal_lo[ipol][ichan];

      // convert to Stokes parameters
      Stokes< Estimate<double> > stokes = convert (cal);
      stokes *= 2.0;

      // convert to MeasuredState format
      Calibration::MeasuredState state;
      for (ipol = 0; ipol<npol; ipol++)
	state.val[ipol] = stokes[ipol].val;
      state.var = stokes[0].var;

      state.source_index = calibrator.source_index;

      Calibration::Measurements measurements ( time.new_Value(epoch) );
      measurements.push_back (state);

      Calibration::ProductTransformation* new_path = 0;

      new_path = model[ichan]->pcal_path->clone();
      new_path->add_Transformation (new Calibration::Gain);

      model[ichan]->equation->add_path (new_path);
      model[ichan]->equation->add_data (measurements);

      if (polcal) {
	Jones<double> caltor = inv (polcal->model[ichan].evaluate());

	Estimate<Stokes<float>, float> calcal;
	calcal.val = caltor * state.val * herm(caltor);
	calcal.var = state.var;

	// add the observed
	calibrator.source[ichan].mean += calcal;
      }

    }
  }


  if (polcal && polcal->model.size() == nchan)  {
    cerr << "Pulsar::ReceptionCalibrator::add_PolnCalibrator"
	" add Polar Model" << endl;

    assert (model.size() == nchan);

    for (unsigned ichan = 0; ichan<nchan; ichan++)
      model[ichan]->polar->integrate( polcal->model[ichan] );
  }

}


//! Add the specified FluxCalibrator observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_FluxCalibrator (const FluxCalibrator* f)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_FluxCalibrator");

  cerr << "Pulsar::ReceptionCalibrator::add_FluxCalibrator unimplemented"
       << endl;

  // FluxCalibrator_path = true;
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::precalibrate (Archive* data)
{
  cerr << "Pulsar::ReceptionCalibrator::precalibrate" << endl;
  calibrate (data, false);
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::calibrate (Archive* data)
{
  cerr << "Pulsar::ReceptionCalibrator::calibrate" << endl;
  calibrate (data, true);
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::calibrate (Archive* data, bool solve_first)
{
  if (!is_fit && solve_first)
    solve ();

  string reason;
  if (!uncalibrated->calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::calibrate",
		 "'" + data->get_filename() + "' does not match "
		 "'" + uncalibrated->get_filename() + "'" + reason);


  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  assert (nchan == model.size());

  vector< Jones<float> > response (nchan);

  bool parallactic_corrected = false;

  for (unsigned isub=0; isub<nsub; isub++) {

    Integration* integration = data->get_Integration (isub);

    time.send( integration->get_epoch() );

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      Calibration::Transformation* signal_path = 0;

      switch ( data->get_type() )  {
      case Signal::Pulsar:
	signal_path = model[ichan]->pulsar_path;
	parallactic_corrected = true;
	break;
      case Signal::PolnCal:
	signal_path = model[ichan]->pcal_path;
	break;
      default:
	throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::calibrate",
		     "unknown Archive type for " + data->get_filename() );
      }

      response[ichan] = inv( signal_path->evaluate() );

    }

    Calibrator::calibrate (integration, response);

    if (isub == 0)
      data->set_state (integration->get_state());
    
  }


  data->set_parallactic_corrected (parallactic_corrected);

  data->set_poln_calibrated (true);

  // if (FluxCalibrator_path)
  // data->set_flux_calibrated (true);
}


bool Pulsar::ReceptionCalibrator::get_solved () const
{
  return is_fit;
}

void Pulsar::ReceptionCalibrator::solve (int only_ichan)
{
  if (!is_initialized)
  check_ready ("Pulsar::ReceptionCalibrator::solve");

  bool degenerate_rotV = false;

  if (calibrator.source.size() == 0) {

    //if (!FluxCalibrator_path)
    //throw Error (InvalidState, "Pulsar::ReceptionCalibrator::solve",
    //	   "no PolnCalibrator or FluxCalibrator data available");

    cerr << "Pulsar::ReceptionCalibrator::solve warning:\n"
      " Without a PolnCalibrator, there remains a degeneracy"
      " along the Stokes V axis" << endl;

    degenerate_rotV = true;
  }

  initialize ();

  unsigned nchan = model.size();

  unsigned start_chan = 0;

  if (only_ichan >= 0)
    start_chan = only_ichan;
#if 0
  else
    cerr << "Pulsar::ReceptionCalibrator::solve CPSR-II aliasing issue:\n"
          "WARNING not solving the first " << start_chan << " channels" <<endl;
#endif

  for (unsigned ichan=start_chan; ichan<nchan; ichan+=1) try {

    cerr << "Pulsar::ReceptionCalibrator::solve ichan=" << ichan << endl;

    model[ichan]->equation->solve ();

    if (only_ichan >= 0)
      break;

  }
  catch (Error& error) {
    throw error += "Pulsar::ReceptionCalibrator::solve";
  }

  is_fit = true;
}

void Pulsar::ReceptionCalibrator::initialize ()
{
  if (is_initialized)
    return;

  PA_min *= 180.0/M_PI;
  PA_max *= 180.0/M_PI;

  cerr << "Pulsar::ReceptionCalibrator::solve information:\n"
    "  Parallactic angle ranges from " << PA_min <<
    " to " << PA_max << " degrees" << endl;

  calibrator.update_source();

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    pulsar[istate].update_source ();

  for (unsigned ichan=0; ichan<model.size(); ichan++)
    model[ichan]->update ();

  is_initialized = true;
}

void Pulsar::ReceptionCalibrator::check_ready (const char* method, bool unc)
{
  if (is_fit)
    throw Error (InvalidState, method,
		 "Model has been fit. Cannot add data.");

  if (is_initialized)
    throw Error (InvalidState, method,
		 "Model has been initialized. Cannot add data.");

  if (unc && !uncalibrated)
    throw Error (InvalidState, method,
		 "Initial observation required.");
}


/*! Update the best guess of each unknown input state */
void Pulsar::SourceEstimate::update_source ()
{
  for (unsigned ichan=0; ichan < source.size(); ichan++)
    source[ichan].update();
}
