#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/CorrectionsCalibrator.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfileFit.h"

#include "Calibration/Instrument.h"
#include "Calibration/MeanInstrument.h"
#include "Calibration/Complex2Math.h"

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type model)
{
  model_type = model;
  maximum_harmonic = 0;
  mean_solution = true;
  integrations_added = 0;
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
Pulsar::PolnCalibrator::Info* Pulsar::PulsarCalibrator::get_Info () const
{
  throw Error (InvalidState, "Pulsar::PulsarCalibrator::get_Info",
	       "not implemented");
}

void Pulsar::PulsarCalibrator::set_maximum_harmonic (unsigned max)
{
  maximum_harmonic = max;
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

  if (!data->get_instrument_corrected ())
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has not been corrected for instrumental configuration");
		 
  if (!data->get_poln_calibrated ())
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has not been calibrated");

  calibrator = data->clone();

  unsigned nchan = calibrator->get_nchan();

  model.resize (nchan);
  transformation.resize (nchan);
  solution.resize (nchan);

  const Integration* integration = calibrator->get_Integration (0);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    cerr << "Pulsar::PulsarCalibrator::set_standard ichan=" << ichan << endl;

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

    if (maximum_harmonic)
      model[ichan]->set_maximum_harmonic( maximum_harmonic );

    model[ichan]->set_standard ( integration->new_PolnProfile (ichan) );
    model[ichan]->set_transformation ( transformation[ichan] * &corrections );

  }

  integrations_added = 0;

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard exit" << endl;

}

//! Add the observation to the set of constraints
void Pulsar::PulsarCalibrator::add_observation (const Archive* data)
{
  if (!data)
    return;

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
		 "mismatch between calibrator\n\t" 
		 + calibrator->get_filename() +
                 " and\n\t" + data->get_filename() + reason);
		 
  if (data->get_poln_calibrated ())
    cerr << "Pulsar::PulsarCalibrator::add_observation warning:\n"
      "  data has alreayd been calibrated" << endl;

  CorrectionsCalibrator correct;

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);

    Jones<double> jones;
    jones = correct.get_transformation( data, isub );
    corrections.set_value( jones );

    if (integrations_added)
      update_solution ();

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

      cerr << "Pulsar::PulsarCalibrator::add_observation" << endl;
      cerr << "  ichan=" << ichan << endl;

      model[ichan]->fit( integration->new_PolnProfile (ichan) );

      cerr << "  gain = " << transformation[ichan]->get_Estimate(0) << endl;
      cerr << "  phase = " << model[ichan]->get_phase() << endl;

      solution[ichan]->integrate( transformation[ichan] );

      if (!integrations_added && ichan+1 < nchan && transformation[ichan+1])
        transformation[ichan+1]->copy( transformation[ichan] );

    }
    catch (Error& error) {
      cerr << "Pulsar::PulsarCalibrator::add_observation ichan="
	   << ichan << " error" << error << endl;
    }

    integrations_added ++;

  }

}

//! Set the flag to return the mean solution or the last fit
void Pulsar::PulsarCalibrator::set_return_mean_solution (bool return_mean)
{
  mean_solution = return_mean;
}

void Pulsar::PulsarCalibrator::update_solution ()
{
  unsigned nchan = model.size ();
  for (unsigned ichan=0; ichan < nchan; ichan++)
    if (model[ichan])
      solution[ichan]->update( transformation[ichan] );
}

   
//! Initialize the PolnCalibration::transformation attribute
void Pulsar::PulsarCalibrator::calculate_transformation ()
{
  // if the mean solution is not required, then the last
  // transformation calculated will be returned
  if (!mean_solution)
    return;

  unsigned nchan = model.size();

  transformation.resize( nchan );

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (transformation[ichan])
      solution[ichan]->update(transformation[ichan]);

}
