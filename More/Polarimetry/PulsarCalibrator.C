#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/CorrectionsCalibrator.h"

#include "Pulsar/ArchiveMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfileFit.h"

#include "Calibration/Instrument.h"
#include "Calibration/MeanInstrument.h"
#include "MEAL/Complex2Math.h"

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type model)
{
  model_type = model;
  maximum_harmonic = 0;
  choose_maximum_harmonic = false;
  mean_solution = true;
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

void Pulsar::PulsarCalibrator::set_choose_maximum_harmonic (bool flag)
{
  choose_maximum_harmonic = flag;
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

  if (!data->get_poln_calibrated ())
    throw Error (InvalidParam,
		 "Pulsar::PulsarCalibrator::set_standard",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has not been calibrated");

  Reference::To<Archive> clone;

  calibrator = clone = data->clone();

  if (!data->get_instrument_corrected ()) {
    cerr << "Pulsar::PulsarCalibrator::set_standard correcting instrument" 
         << endl;
    clone->correct_instrument ();
  }

  unsigned nchan = calibrator->get_nchan();

  model.resize (nchan);
  transformation.resize (nchan);
  solution.resize (nchan);

  const Integration* integration = calibrator->get_Integration (0);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::set_standard ichan=" << ichan << endl;

    transformation[ichan] = 0;
    solution[ichan] = 0;
    model[ichan] = 0;

    if (integration->get_weight (ichan) == 0) {
      cerr << "Pulsar::PulsarCalibrator::set_standard ichan="
	   << ichan << " flagged invalid" << endl;
      continue;
    }

    model[ichan] = new PolnProfileFit;

    if (choose_maximum_harmonic)
      model[ichan]->choose_maximum_harmonic = true;
    else if (maximum_harmonic)
      model[ichan]->set_maximum_harmonic( maximum_harmonic );

    model[ichan]->set_standard ( integration->new_PolnProfile (ichan) );

  }

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard exit" << endl;

}

double chisq (const MEAL::Function* a, const MEAL::Function* b,
	      unsigned start = 0)
{
  unsigned nparam = a->get_nparam();
  if (nparam != b->get_nparam())
    throw Error (InvalidParam, "chisq",
		 "a.nparam=%u != b.nparam=%u", nparam, b->get_nparam());

  double chisq = 0.0;
  for (unsigned iparam=start; iparam<nparam; iparam++) {
    double diff = a->get_param(iparam) - b->get_param(iparam);
    double var = a->get_variance(iparam) + b->get_variance(iparam);
    if (var != 0.0)
      chisq += diff*diff/var;
  }

  return chisq/nparam;
}

//! Add the observation to the set of constraints
void Pulsar::PulsarCalibrator::add_observation (const Archive* data)
{
  if (!data)
    return;

  if (!calibrator)
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::add_observation",
		 "no calibrator");

  ArchiveMatch match;

  match.set_check_standard (true);
  match.set_check_calibrator (true);
  match.set_check_nbin (false);

  if (!match.match (calibrator, data))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_observation",
                 "mismatch between calibrator\n\t"
                 + calibrator->get_filename() +
                 " and\n\t" + data->get_filename() + match.get_reason());

  if (data->get_poln_calibrated ())
    cerr << "Pulsar::PulsarCalibrator::add_observation warning:\n"
      "  data has already been calibrated" << endl;

  CorrectionsCalibrator correct;

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);

    Jones<double> jones;
    jones = correct.get_transformation( data, isub );
    corrections.set_value( jones );

    for (unsigned ichan=0; ichan<nchan; ichan++)
      solve (integration, ichan);

  }

}

void Pulsar::PulsarCalibrator::solve (const Integration* data, unsigned ichan)
{
  if (!model[ichan]) {
    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve standard ichan="
	   << ichan << " flagged invalid" << endl;
    return;
  }

  if (data->get_weight (ichan) == 0) {
    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve ichan="
	   << ichan << " flagged invalid" << endl;
    transformation[ichan] = 0;
    return;
  }

  float reduced_chisq = 0.0;

  for (unsigned tries=0 ; tries < 2; tries ++) try {

    if (!transformation[ichan]) {
      transformation[ichan] = new Calibration::Instrument;
      model[ichan]->set_transformation (transformation[ichan] * &corrections);
    }

    if (solution[ichan])
      solution[ichan]->update( transformation[ichan] );
    else if (ichan>0 && tries==0 && solution[ichan-1])
      solution[ichan-1]->update( transformation[ichan] );

    model[ichan]->fit( data->new_PolnProfile (ichan) );

    unsigned iterations = model[ichan]->get_fit_iterations ();
    unsigned nfree = model[ichan]->get_fit_nfree ();
    float chisq = model[ichan]->get_fit_chisq ();

    reduced_chisq = chisq / nfree;

    if (reduced_chisq < 1.1)
      break;

    cerr << "Pulsar::PulsarCalibrator::solve ichan=" << ichan
	 << " invalid reduced chisq=" << reduced_chisq << endl;

    // try again with a fresh start
    transformation[ichan] = 0;

    if (!solution[ichan])
      break;

    solution[ichan] = 0;
  }
  catch (Error& error) {
    cerr << "Pulsar::PulsarCalibrator::solve ichan="
	 << ichan << " error" << error << endl;
    transformation[ichan] = 0;
    solution[ichan] = 0;
  }

  if (!transformation[ichan])
    return;

#ifdef _DEBUG
  for (unsigned ip=0; ip < transformation[ichan]->get_nparam(); ip++)
    cerr << "  " << ip << " " << transformation[ichan]->get_Estimate(ip)
	 << endl;

  cerr << "  chisq=" << reduced_chisq << " phase = " 
       << model[ichan]->get_phase() << endl;
#endif

  if (solution[ichan]) {

    Calibration::Instrument test;
    solution[ichan]->update (&test);

    float solution_chisq = chisq( &test, transformation[ichan], 1 );
    if (solution_chisq > 3.0) {
      cerr << "  BIG DIFFERENCE=" << solution_chisq << endl;
      cerr << "    OLD\t\t\t\tNEW" << endl;

      unsigned nparam = test.get_nparam();
      for (unsigned ip=1; ip < nparam; ip++)
	cerr << "  " << ip << " " << test.get_Estimate(ip)
	     << "\t\t\t\t" << transformation[ichan]->get_Estimate(ip) << endl;

      solution[ichan] = 0;
    }

  }

  if (!solution[ichan])
    solution[ichan] = new Calibration::MeanInstrument;  
  
  solution[ichan]->integrate( transformation[ichan] );
  
}


//! Set the flag to return the mean solution or the last fit
void Pulsar::PulsarCalibrator::set_return_mean_solution (bool return_mean)
{
  mean_solution = return_mean;
}

void Pulsar::PulsarCalibrator::update_solution ()
{
  unsigned nchan = model.size ();
  for (unsigned ichan=0; ichan < nchan; ichan++) {
    if (solution[ichan]) {
      if (!transformation[ichan])
	transformation[ichan] = new Calibration::Instrument;
      solution[ichan]->update( transformation[ichan] );
    }
  }
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
  update_solution ();
}
