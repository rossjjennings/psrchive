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

  float latitude = 0;
  uncalibrated->telescope_coordinates (&latitude);
  parallactic.set_observatory_latitude( latitude * M_PI/180.0 );

  sky_coord position = uncalibrated->get_coordinates();
  parallactic.set_source_declination( position.dec().getRadians() );
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

  pulsar.push_back ( PhaseEstimate (ibin) );
  add_data (pulsar.back(), uncalibrated);
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


void Pulsar::ReceptionCalibrator::fit ()
{
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
    equation[ichan].model.add_transformation (&parallactic);

    // for (unsigned istate=0; istate<pulsar.size(); istate++)
      // pulsar[istate].states[ichan].get_Estimate();
  }
}

void Pulsar::ReceptionCalibrator::add_data (PhaseEstimate& estimate,
					    const Archive* data)
{
  unsigned nchan = data->get_nchan ();
  estimate.states.resize (nchan);

  unsigned nsub = data->get_nsubint ();

  unsigned ibin = estimate.phase_bin;

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);
    Estimate<Stokes<float>, float> stokes;

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      stokes.val = integration->get_Stokes ( ichan, ibin, &(stokes.var) );

      // TODO: undo parallactic angle transformation

      estimate.states[ichan] += stokes;
//Estimate<Stokes<double>, double>(stokes);

    }
  }
}


void Pulsar::ReceptionCalibrator::check_fixed (const char* method)
{
  if (is_fixed())
    throw Error (InvalidState, method, "Model has been fit. Cannot add data.");
}
