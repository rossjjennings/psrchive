#include "ReceptionCalibrator.h"
#include "Archive.h"

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
}


//! Add the specified pulse phase bin to the set of state constraints
void Pulsar::ReceptionCalibrator::add_state (float pulse_phase)
{

}


//! Get the number of pulse phase bin state constraints
unsigned Pulsar::ReceptionCalibrator::get_nstate () const
{
  return 0;
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_observation (const Archive* data)
{
  string reason;

  if (!uncalibrated->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "Pulsar::Archive='" + data->get_filename() +
		 "'\ndoes not mix with '" + uncalibrated->get_filename() + 
		 "\n" + reason);

}

//! Add the specified PolnCalibrator observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_PolnCalibrator (const PolnCalibrator* p)
{
  cerr << "Pulsar::ReceptionCalibrator::add_PolnCalibrator unimplemented"
       << endl;
}

//! Add the specified FluxCalibrator observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_FluxCalibrator (const FluxCalibrator* f)
{
  cerr << "Pulsar::ReceptionCalibrator::add_FluxCalibrator unimplemented"
       << endl;
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::calibrate (Archive* archive)
{
  cerr << "Pulsar::ReceptionCalibrator::calibrate unimplemented" << endl;
}

