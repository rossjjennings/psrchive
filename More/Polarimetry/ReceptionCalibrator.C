#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
Pulsar::ReceptionCalibrator::ReceptionCalibrator (const Archive* archive)
{
  is_fit = false;
  ncoef = 0;
  ncoef_set = false;

  includes_PolnCalibrator = false;
  includes_FluxCalibrator = false;

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
  
  // Check that the archive has full polarization information
  Signal::State state = data->get_state();
  bool fullStokes = state == Signal::Stokes || state == Signal::Coherence;

  if (!fullStokes)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "invalid state=" + State2string(state));

  if (data->get_parallactic_corrected ())
    throw Error (InvalidParam,"Pulsar::ReceptionCalibrator::initial_observation",
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

  Stokes<double> cal_state (1,0,1,0);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    calibrator[ichan].set_stokes (cal_state);
    for (unsigned istokes=0; istokes<4; istokes++)
      calibrator[ichan].set_infit (istokes, false);
    
    equation[ichan] = new Calibration::SAtPEquation;

    // add the calibrator state before the parallactic angle transformation
    equation[ichan]->model.add_state( &(calibrator[ichan]) );
    equation[ichan]->model.add_transformation( &parallactic );

  }

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    init_estimate ( pulsar[istate] );

  start_epoch = end_epoch = data->start_time ();
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

  pulsar.push_back( PhaseEstimate (phase_bin) );

  if (uncalibrated)
    init_estimate( pulsar.back() );
}

void Pulsar::ReceptionCalibrator::init_estimate (PhaseEstimate& estimate)
{
  unsigned nchan = uncalibrated->get_nchan ();
  unsigned nbin = uncalibrated->get_nbin ();

  if (estimate.phase_bin >= nbin)
    throw Error (InvalidRange, "Pulsar::ReceptionCalibrator::init_estimate",
		 "phase bin=%d >= nbin=%d", estimate.phase_bin, nbin);

  estimate.mean.resize (nchan);
  estimate.state.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
    equation[ichan]->model.add_state( &(estimate.state[ichan]) );
}



//! Get the number of pulse phase bin state constraints
unsigned Pulsar::ReceptionCalibrator::get_nstate () const
{
  return 0;
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_observation (const Archive* data)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_observation", false);

  if (!uncalibrated)
    initial_observation (data);

  string reason;
  if (!uncalibrated->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "Archive='" + data->get_filename() + "'\ndoes not match '"
		 + uncalibrated->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    parallactic.set_epoch (epoch);

    if (epoch < start_epoch)
      start_epoch = epoch;
    if (epoch > end_epoch)
      end_epoch = epoch;

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      // the selected pulse phase bins
      vector<Calibration::MeasuredState> measurements;

      for (unsigned istate=0; istate < pulsar.size(); istate++) {
	add_data (measurements, pulsar[istate], ichan, integration);
	measurements.back().state_index = istate + 1;
      }

      equation[ichan]->add_measurement (epoch, measurements);
    }
  }
}

void
Pulsar::ReceptionCalibrator::add_data(vector<Calibration::MeasuredState>& bins,
				      PhaseEstimate& estimate,
				      unsigned ichan,
				      const Integration* data)
{
  unsigned nchan = data->get_nchan ();

  // sanity check
  if (estimate.mean.size () != nchan)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_data",
		 "PhaseEstimate.nchan=%d != Integration.nchan=%d",
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

  cerr << "Pulsar::ReceptionCalibrator::add_PolnCalibrator unimplemented"
       << endl;

  // includes_PolnCalibrator = true;
}

//! Add the specified FluxCalibrator observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_FluxCalibrator (const FluxCalibrator* f)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_FluxCalibrator");

  cerr << "Pulsar::ReceptionCalibrator::add_FluxCalibrator unimplemented"
       << endl;

  // includes_FluxCalibrator = true;
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
		 "Pulsar::Archive='" + data->get_filename() +
		 "'\ndoes not match '" + uncalibrated->get_filename() + reason);

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
      response[ichan] = equation[ichan]->model.get_Jones();
    }

    Calibrator::calibrate (integration, response);

    if (isub == 0)
      data->set_state (integration->get_state());
    
  }

  data->set_parallactic_corrected (true);
  data->set_poln_calibrated (true);

  if (includes_FluxCalibrator)
    data->set_flux_calibrated (true);
}


void Pulsar::ReceptionCalibrator::solve ()
{
  check_ready ("Pulsar::ReceptionCalibrator::solve");

  if (!ncoef_set) {
    /* it might be nice to try and choose a good ncoef, based on the
       timescale on which the backend is expected to change and the
       amount of time spanned by the observations */
  }

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    pulsar[istate].update_state ();

  MJD mid = 0.5 * (start_epoch + end_epoch);

  for (unsigned ichan=0; ichan<equation.size(); ichan++) try {

    cerr << "Pulsar::ReceptionCalibrator::solve ichan=" << ichan << endl;

    if (ncoef)
      equation[ichan]->set_ncoef (ncoef);

    equation[ichan]->set_reference_epoch (mid);

    equation[ichan]->solve ();
  }
  catch (Error& error) {
    throw error += "Pulsar::ReceptionCalibrator::solve";
  }

  is_fit = true;
}



void Pulsar::ReceptionCalibrator::check_ready (const char* method, bool unc)
{
  if (is_fit)
    throw Error (InvalidState, method, "Model has been fit. Cannot add data.");

  if (unc && !uncalibrated)
    throw Error (InvalidState, method, "Initial observation required.");
}


/*! Update the best guess of each unknown input state */
void Pulsar::PhaseEstimate::update_state ()
{
  for (unsigned ichan=0; ichan < state.size(); ichan++)
    state[ichan].set_stokes( mean[ichan].get_Estimate().val );
}
