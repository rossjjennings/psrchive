#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

#include "Calibration/Instrument.h"
#include "Calibration/MeanInstrument.h"
#include "Calibration/Complex2Math.h"

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type model)
{
  model_type = model;
}

//! Constructor
Pulsar::PulsarCalibrator::~PulsarCalibrator ()
{
}

//! Return the reference epoch of the calibration experiment
MJD Pulsar::PulsarCalibrator::get_epoch () const
{
  if (!calibrator) 
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::get_epoch",
		 "no calibrator");

  return calibrator->start_time();
}

//! Return Calibrator::Hamaker or Calibrator::Britton
Pulsar::Calibrator::Type Pulsar::PulsarCalibrator::get_type () const
{
  return model_type;
}

//! Return the Calibrator information
Pulsar::Calibrator::Info* Pulsar::PulsarCalibrator::get_Info () const
{
  throw Error (InvalidState, "Pulsar::PulsarCalibrator::get_Info",
	       "not implemented");
}

void Pulsar::PulsarCalibrator::set_standard (const Archive* data)
{
  if (!data)
    throw Error (InvalidState, "PulsarCalibrator::set_standard",
		 "no Archive");

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard" << endl;

  if (data->get_type() != Signal::Pulsar)
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='" + data->get_filename() 
		 + "' not a Pulsar observation");

  if (data->get_state() != Signal::Stokes)
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='%s' state=%s != Signal::Stokes",
		 data->get_filename().c_str(),
		 Signal::state_string(data->get_state()));

  if (!data->get_parallactic_corrected ())
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has not been corrected for parallactic angle rotation");
		 
  if (!data->get_poln_calibrated ())
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has not been calibrated");

  calibrator = data->clone();

  float latitude = 0, longitude = 0;
  calibrator->telescope_coordinates (&latitude, &longitude);
  sky_coord coordinates = calibrator->get_coordinates();

  parallactic.set_source_coordinates( coordinates );
  parallactic.set_observatory_coordinates (latitude,longitude);

  unsigned nchan = calibrator->get_nchan();

  model.resize (nchan);
  transformation.resize (nchan);
  solution.resize (nchan);

  const Integration* integration = calibrator->get_Integration (0);

  // the noise power in the profile baselines are used to estimate the
  // variance in each Stokes parameter
  vector< vector< double > > baseline_variance;
  integration->baseline_stats (0, &baseline_variance);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    if (integration->get_weight (ichan) == 0) {
      cerr << "Pulsar::PulsarCalibrator::set_standard ichan="
	   << ichan << " flagged invalid" << endl;
      transformation[ichan] = 0;
      solution[ichan] = 0;
      model[ichan] = 0;
      continue;
    }

    transformation[ichan] = new Calibration::Instrument;
    solution[ichan] = new Calibration::MeanInstrument;
    model[ichan] = new PolnProfileFit;

    model[ichan]->set_standard ( integration->new_PolnProfile (ichan) );
    model[ichan]->set_transformation ( transformation[ichan] * &parallactic );

  }

}

//! Add the observation to the set of constraints
void Pulsar::PulsarCalibrator::add_observation (const Archive* data)
{
  if (!data)
    return;

  if (data->get_type() == Signal::PolnCal) {
    add_calibrator (data);
    return;
  }

  if (!calibrator)
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::add_observation",
		 "no calibrator");

  string reason;
  if (!calibrator->standard_match (data, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_observation",
		 "'" + data->get_filename() + "' does not match "
		 "'" + calibrator->get_filename() + reason);

  if (!calibrator->calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_observation",
                 "'" + data->get_filename() + "' does not match "
                 "'" + calibrator->get_filename() + reason);
		 
  if (data->get_poln_calibrated ())
    cerr << "Pulsar::PulsarCalibrator::add_observation warning:\n"
      "  data has alreayd been calibrated" << endl;

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);

    if (!data->get_parallactic_corrected ())
      parallactic.set_epoch( integration->get_epoch () );
    else
      parallactic.set_phi( 0 );

    // the noise power in the baseline is used to estimate the
    // variance in each Stokes parameter
    vector< vector< double > > baseline_variance;
    integration->baseline_stats (0, &baseline_variance);

    for (unsigned ichan=0; ichan<nchan; ichan++) try {

      if (integration->get_weight (ichan) == 0) {
	cerr << "Pulsar::PulsarCalibrator::add_observation ichan="
	     << ichan << " flagged invalid" << endl;
	continue;
      }

      if (!model[ichan]) {
        cerr << "Pulsar::PulsarCalibrator::add_observation standard ichan="
             << ichan << " flagged invalid" << endl;
        continue;
      }

      model[ichan]->fit( integration->new_PolnProfile (ichan) );
      solution[ichan]->integrate( transformation[ichan] );

      if (ichan+1 < nchan)
        transformation[ichan+1]->copy( transformation[ichan] );

    }
    catch (Error& error) {
      cerr << "Pulsar::PulsarCalibrator::add_observation ichan="
	   << ichan << " error" << error << endl;
    }

  }
}

void Pulsar::PulsarCalibrator::update_solution ()
{
  unsigned nchan = model.size ();
  for (unsigned ichan=0; ichan < nchan; ichan++)
    if (model[ichan])
      solution[ichan]->update( transformation[ichan] );
}

//! Add the calibrator observation to the set of constraints
void Pulsar::PulsarCalibrator::add_calibrator (const Archive* data)
{
}
    
//! Add the ArtificialCalibrator observation to the set of constraints
void 
Pulsar::PulsarCalibrator::add_calibrator (const ArtificialCalibrator* polncal)
{
}
   
//! Initialize the PolnCalibration::transformation attribute
void Pulsar::PulsarCalibrator::calculate_transformation ()
{
  unsigned nchan = model.size();

  transformation.resize( nchan );

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (transformation[ichan])
      solution[ichan]->update(transformation[ichan]);

}
