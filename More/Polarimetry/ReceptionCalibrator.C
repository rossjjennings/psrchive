#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
Pulsar::ReceptionCalibrator::ReceptionCalibrator (const Archive* archive)
{
  is_fit = false;
  is_initialized = false;

  ncoef = 0;
  ncoef_set = false;
 
  PolnCalibrator_path = 0;
  FluxCalibrator_path = 0;
  calibrator_state_index = 0;

  PA_min = PA_max = 0.0;

  if (archive)
    initial_observation (archive);
}

void Pulsar::ReceptionCalibrator::set_ncoef (unsigned _ncoef)
{
  if (is_fit)
    cerr << "Pulsar::ReceptionCalibrator::set_ncoef ignored." << endl;

  ncoef = _ncoef;
  ncoef_set = true;
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


  uncalibrated = data;

  parallactic.set_source_coordinates( uncalibrated->get_coordinates() );

  float latitude = 0, longitude = 0;
  uncalibrated->telescope_coordinates (&latitude, &longitude);
  parallactic.set_observatory_coordinates (latitude, longitude);

  unsigned nchan = uncalibrated->get_nchan();

  equation.resize (nchan);
  calibrator.resize (nchan);
  receiver.resize (nchan);

  Stokes<double> cal_state (1,0,1,0);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    calibrator[ichan].set_stokes (cal_state);
    for (unsigned istokes=0; istokes<4; istokes++)
      calibrator[ichan].set_infit (istokes, false);
    
    equation[ichan] = new Calibration::SAtPEquation;

    // add the calibrator state before the parallactic angle transformation
    equation[ichan]->get_model()->add_state( &(calibrator[ichan]) );
    equation[ichan]->get_model()->add_transformation( &parallactic );

  }

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    init_estimate ( pulsar[istate] );

  start_epoch = end_epoch = data->start_time ();

  parallactic.set_epoch (start_epoch);
  PA_max = PA_min = parallactic.get_param (0);

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

  estimate.mean.resize (nchan);
  estimate.state.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
    equation[ichan]->get_model()->add_state( &(estimate.state[ichan]) );
}



//! Get the number of pulse phase bin state constraints
unsigned Pulsar::ReceptionCalibrator::get_nstate () const
{
  return pulsar.size();
}

unsigned Pulsar::ReceptionCalibrator::get_nchan () const
{
  return equation.size();
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_observation (const Archive* data)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_observation", false);

  if (data->get_type() == Signal::PolnCal) {
    Reference::To<PolnCalibrator> polarcal = new PolarCalibrator (data);
    polarcal->build( uncalibrated->get_nchan() );
    add_PolnCalibrator (polarcal);
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
      vector<Calibration::MeasuredState> measurements;

      for (unsigned istate=0; istate < pulsar.size(); istate++) {
	add_data (measurements, pulsar[istate], ichan, integration);
	measurements.back().state_index = istate;
      }

      equation[ichan]->add_measurement (epoch, measurements);
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
  if (estimate.mean.size () != nchan)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_data",
		 "SourceEstimate.nchan=%d != Integration.nchan=%d",
		 estimate.mean.size(), nchan);

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

  // Correct for parallactic angle rotation before adding to best estimate
  Jones<double> unpara = inv( parallactic.evaluate() );
  stokes.val = unpara * stokes.val * herm(unpara);
  estimate.mean[ichan] += stokes;

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

  if (!PolnCalibrator_path) {

    // this is the first calibrator observation
    calibrator.resize (nchan);

    // add the calibrator state
    Stokes<double> cal_state (1,0,1,0);

    calibrator_state_index = equation[0]->get_model()->get_nstate ();

    PolnCalibrator_path = equation[0]->get_nbackend();

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      calibrator[ichan].set_stokes (cal_state);
      for (unsigned istokes=0; istokes<4; istokes++)
	calibrator[ichan].set_infit (istokes, false);
    
      // add the calibrator states to a new signal path
      equation[ichan]->add_backend ();

      equation[ichan]->get_model()->add_state( &(calibrator[ichan]) );

    }

  }

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

      // convert to MeasuredState format
      Calibration::MeasuredState state;
      for (ipol = 0; ipol<npol; ipol++)
	state.val[ipol] = stokes[ipol].val;
      state.var = stokes[0].var;

      state.state_index = calibrator_state_index;

      equation[ichan]->add_measurement (epoch, state);

    }
  }

  const PolarCalibrator* polcal = dynamic_cast<const PolarCalibrator*>(p);

  if (polcal && polcal->model.size() == nchan)  {
    cerr << "Pulsar::ReceptionCalibrator::add_PolnCalibrator"
	" add Polar Model" << endl;

    assert (receiver.size() == nchan);

    for (unsigned ichan = 0; ichan<nchan; ichan++)
      receiver[ichan].integrate( polcal->model[ichan] );
  }

}

void Pulsar::PolarEstimate::integrate (const Calibration::Polar& model)
{
  gain += model.get_gain ();

  for (unsigned i=0; i<3; i++) {
    boostGibbs[i] += model.get_boostGibbs (i);
    rotationEuler[i] += model.get_rotationEuler (i);
  }
}

void Pulsar::PolarEstimate::update (Calibration::Polar* model)
{
  model->set_gain (gain.get_Estimate());

  for (unsigned i=0; i<3; i++) {
    model->set_boostGibbs (i, boostGibbs[i].get_Estimate());
    model->set_rotationEuler (i, rotationEuler[i].get_Estimate());
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
void Pulsar::ReceptionCalibrator::calibrate (Archive* data)
{
  cerr << "Pulsar::ReceptionCalibrator::calibrate" << endl;

  if (!is_fit)
    solve ();

  string reason;
  if (!uncalibrated->calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::calibrate",
		 "'" + data->get_filename() + "' does not match "
		 "'" + uncalibrated->get_filename() + "'" + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  assert (nchan == equation.size());

  vector< Jones<float> > response (nchan);

  for (unsigned isub=0; isub<nsub; isub++) {

    Integration* integration = data->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    for (unsigned ichan=0; ichan<nchan; ichan++) {
      equation[ichan]->set_epoch (epoch);
      response[ichan] = inv( equation[ichan]->get_model()->get_Jones() );
    }

    Calibrator::calibrate (integration, response);

    if (isub == 0)
      data->set_state (integration->get_state());
    
  }

  data->set_parallactic_corrected (true);
  data->set_poln_calibrated (true);

  if (FluxCalibrator_path)
    data->set_flux_calibrated (true);
}


void Pulsar::ReceptionCalibrator::solve ()
{
  check_ready ("Pulsar::ReceptionCalibrator::solve");

  bool degenerate_rotV = false;

  if (!PolnCalibrator_path) {

    if (!FluxCalibrator_path)
      throw Error (InvalidState, "Pulsar::ReceptionCalibrator::solve",
		   "no PolnCalibrator or FluxCalibrator data available");

    cerr << "Pulsar::ReceptionCalibrator::solve warning:\n"
      " Without a PolnCalibrator, there remains a degeneracy"
      " along the Stokes V axis" << endl;

    degenerate_rotV = true;
  }

  if (!ncoef_set) {
    /* it might be nice to try and choose a good ncoef, based on the
       timescale on which the backend is expected to change and the
       amount of time spanned by the observations */
  }

  initialize ();

  unsigned nchan = equation.size();
  unsigned incr = 1;

  for (unsigned ichan=0; ichan<nchan; ichan+=incr) try {

    cerr << "Pulsar::ReceptionCalibrator::solve ichan=" << ichan << endl;

    if (ncoef)
      equation[ichan]->set_ncoef (ncoef);

    if (degenerate_rotV) {
      equation[ichan]->get_receiver()->set_param (6, 0.0);
      equation[ichan]->get_receiver()->set_infit (6, false);
    }

    equation[ichan]->solve ();

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

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    pulsar[istate].update_state ();

  MJD mid = 0.5 * (start_epoch + end_epoch);
  parallactic.set_reference_epoch (mid);

  unsigned nchan = equation.size();

  for (unsigned ichan=0; ichan<nchan; ichan+=1) {

    equation[ichan]->set_reference_epoch (mid);
    receiver[ichan].update (equation[ichan]->get_receiver());

  }

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
void Pulsar::SourceEstimate::update_state ()
{
  for (unsigned ichan=0; ichan < state.size(); ichan++) {
    Estimate<Stokes<double>, double> stokes = mean[ichan].get_Estimate();
    state[ichan].set_stokes( stokes.val, sqrt(stokes.var) );
  }
}
