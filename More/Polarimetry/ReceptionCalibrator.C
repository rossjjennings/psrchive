#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
Pulsar::ReceptionCalibrator::ReceptionCalibrator (const Archive* archive)
{
  if (!archive)
    throw Error (InvalidState, "ReceptionCalibrator::", "no Archive");

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator" << endl;

  if (archive->get_type() != Signal::Pulsar)
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "Pulsar::Archive='" + archive->get_filename() 
		 + "' not a Pulsar observation");
  
  // Here the decision is made about full stokes or dual band observations.
  Signal::State state = archive->get_state();

  bool fullStokes = state == Signal::Stokes || state == Signal::Coherence;

  if (!fullStokes)
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "Pulsar::Archive='" + archive->get_filename() + "'\n"
		 "invalid state=" + State2string(state));

  if (state != Signal::Stokes) {
    Archive* clone = archive->clone();
    clone->convert_state (Signal::Stokes);
    uncalibrated = clone;
  }
  else
    uncalibrated = archive;

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
    
    // add the calibrator state before the parallactic angle transformat
    equation[ichan].model.add_state( &(calibrator[ichan]) );
    equation[ichan].model.add_transformation( &parallactic );

  }

  fixed = false;
}


//! Add the specified pulse phase bin to the set of state constraints
void Pulsar::ReceptionCalibrator::add_state (float phase)
{
  check_fixed ("Pulsar::ReceptionCalibrator::add_state");

  unsigned nbin = uncalibrated->get_nbin ();
  unsigned ibin = unsigned (phase * nbin) % nbin;

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::add_state phase=" << phase 
	 << " bin=" << ibin << endl;

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    if (pulsar[istate].phase_bin == ibin) {
      cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << ibin
	   << " already in use" << endl;
      return;
    }

  pulsar.push_back( PhaseEstimate (ibin) );
  add_estimate( pulsar.back() );
}


//! Get the number of pulse phase bin state constraints
unsigned Pulsar::ReceptionCalibrator::get_nstate () const
{
  return 0;
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_observation (const Archive* data)
{
  check_fixed ("Pulsar::ReceptionCalibrator::add_observation");

  string reason;

  if (!uncalibrated->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "Pulsar::Archive='" + data->get_filename() +
		 "'\ndoes not mix with '" + uncalibrated->get_filename() + 
		 "\n" + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    parallactic.set_epoch (epoch);

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      // the selected pulse phase bins
      vector<Calibration::MeasuredState> measurements;

      for (unsigned istate=0; istate < pulsar.size(); istate++) {
	add_data (measurements, pulsar[istate], ichan, integration);
	measurements.back().state_index = istate + 1;
      }

      equation[ichan].add_measurement (epoch, measurements);
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
  check_fixed ("Pulsar::ReceptionCalibrator::add_PolnCalibrator");

  cerr << "Pulsar::ReceptionCalibrator::add_PolnCalibrator unimplemented"
       << endl;
}

//! Add the specified FluxCalibrator observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_FluxCalibrator (const FluxCalibrator* f)
{
  check_fixed ("Pulsar::ReceptionCalibrator::add_FluxCalibrator");

  cerr << "Pulsar::ReceptionCalibrator::add_FluxCalibrator unimplemented"
       << endl;
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::calibrate (Archive* data)
{
  cerr << "Pulsar::ReceptionCalibrator::calibrate" << endl;

  if (!fixed)
    fit ();

  string reason;
  if (!uncalibrated->match (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::calibrate",
		 "Pulsar::Archive='" + data->get_filename() +
		 "'\ndoes not match '" + uncalibrated->get_filename() + 
		 "\n" + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  assert (nchan == equation.size());

  vector< Jones<float> > response (nchan);

  for (unsigned isub=0; isub<nsub; isub++) {

    Integration* integration = data->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    for (unsigned ichan=0; ichan<nchan; ichan++) {
      equation[ichan].set_epoch (epoch);
      response[ichan] = equation[ichan].model.get_Jones();
    }

    Calibrator::calibrate (integration, response);

    if (isub == 0)
      data->set_state (integration->get_state());
    
  }

  // TODO: set calibrated flags
}


void Pulsar::ReceptionCalibrator::fit ()
{
  if (fixed)
    return;

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    pulsar[istate].update_state ();

  for (unsigned ichan=0; ichan<equation.size(); ichan++)
    equation[ichan].solve ();

  fixed = true;
}


void Pulsar::ReceptionCalibrator::add_estimate (PhaseEstimate& estimate)
{
  unsigned nchan = uncalibrated->get_nchan ();

  estimate.mean.resize (nchan);
  estimate.state.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
    equation[ichan].model.add_state( &(estimate.state[ichan]) );
}

void Pulsar::ReceptionCalibrator::check_fixed (const char* method)
{
  if (is_fixed())
    throw Error (InvalidState, method, "Model has been fit. Cannot add data.");
}

bool Pulsar::ReceptionCalibrator::is_fixed () const
{
  return fixed;
}

/*! Update the best guess of each unknown input state */
void Pulsar::PhaseEstimate::update_state ()
{
  for (unsigned ichan=0; ichan < state.size(); ichan++)
    state[ichan].set_stokes( mean[ichan].get_Estimate().val );
}
