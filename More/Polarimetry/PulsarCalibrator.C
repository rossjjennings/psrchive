/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/CorrectionsCalibrator.h"

#include "Pulsar/ArchiveMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfileFit.h"

#include "Calibration/Instrument.h"
#include "Calibration/MeanInstrument.h"

#include "MEAL/Complex2Math.h"

#include "genutil.h"
#include "toa.h"

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type model)
{
  model_type = model;
  maximum_harmonic = 0;
  choose_maximum_harmonic = false;
  mean_solution = true;
  tim_file = 0;
  archive = 0;
}

//! Constructor
Pulsar::PulsarCalibrator::~PulsarCalibrator ()
{
  if (tim_file)
    fclose (tim_file);
}

//! Return the reference epoch of the calibration experiment
MJD Pulsar::PulsarCalibrator::get_epoch () const
try {
  if (epoch != MJD::zero)
    return epoch;
  else
    return get_calibrator()->start_time();
}
catch (Error& error) {
  error += "Pulsar::PulsarCalibrator::get_epoch";
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

  if (verbose > 2)
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

  set_calibrator( clone = data->clone() );

  if (!data->get_instrument_corrected ()) {
    cerr << "Pulsar::PulsarCalibrator::set_standard correcting instrument" 
         << endl;
    clone->correct_instrument ();
  }

  unsigned nchan = clone->get_nchan();

  model.resize (nchan);
  transformation.resize (nchan);
  solution.resize (nchan);

  const Integration* integration = clone->get_Integration (0);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    if (verbose > 2)
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

  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::set_standard exit" << endl;

}

//! Add the observation to the set of constraints
void Pulsar::PulsarCalibrator::add_observation (const Archive* data) try 
{
  if (!data)
    return;

  ArchiveMatch match;

  match.set_check_standard (true);
  match.set_check_calibrator (true);
  match.set_check_nbin (false);

  if (!match.match (get_calibrator(), data))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_observation",
                 "mismatch between calibrator\n\t"
                 + get_calibrator()->get_filename() +
                 " and\n\t" + data->get_filename() + match.get_reason());

  if (data->get_poln_calibrated ())
    cerr << "Pulsar::PulsarCalibrator::add_observation warning:\n"
      "  data has already been calibrated" << endl;

  CorrectionsCalibrator correct;

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  epoch = data->start_time ();

  if (tim_file)
    archive = data;

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);

    Jones<double> jones;
    jones = correct.get_transformation( data, isub );
    corrections.set_value( jones );

    // if 5% of the solutions diverge from the mean, clear the mean
    unsigned clean_mean = nchan/20;
    big_difference = 0;

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      solve (integration, ichan);

      // the current mean is no longer providing a good first guess; clear it!
      if (big_difference >= clean_mean) {
	cerr << "Pulsar::PulsarCalibrator::add_observation"
	  " clearing the current mean" << endl;
	for (unsigned jchan=ichan+1; jchan<nchan; jchan++)
	  solution[jchan] = 0;
	big_difference = 0;
      }

    }

  }

  built = false;
}
catch (Error& error) {
  throw error += "Pulsar::PulsarCalibrator::add_observation";
}


MEAL::Complex2* Pulsar::PulsarCalibrator::new_transformation () const
{
  Calibration::Instrument* instrument = new Calibration::Instrument;
  instrument->set_cyclic();
  return instrument;
}

void Pulsar::PulsarCalibrator::solve (const Integration* data, unsigned ichan)
{
  if (!model[ichan]) {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::solve standard ichan="
	   << ichan << " flagged invalid" << endl;
    return;
  }

  if (data->get_weight (ichan) == 0) {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::solve ichan="
	   << ichan << " flagged invalid" << endl;
    transformation[ichan] = 0;
    return;
  }

  float reduced_chisq = 0.0;

  for (unsigned tries=0 ; tries < 2; tries ++) try {

    if (!transformation[ichan]) {
      transformation[ichan] = new_transformation();
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

    if (iterations > 10 && solution[ichan])  {
      cerr << "LARGE ITERATIONS=" << iterations << endl;
      solution[ichan] = 0;
    }

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
	 << ichan << " error " << error.get_message() << endl;
    transformation[ichan] = 0;
    solution[ichan] = 0;
  }

  if (!transformation[ichan])
    return;

#ifdef _DEBUG
  for (unsigned ip=0; ip < transformation[ichan]->get_nparam(); ip++)
    cerr << "  " << ip << " " << transformation[ichan]->get_Estimate(ip)
	 << endl;

#endif

  if (tim_file) {

    // produce a TOA!

    Tempo::toa toa (Tempo::toa::Parkes);

    double freq = data->get_centre_frequency (ichan);
    toa.set_frequency (freq);

    double period = data->get_folding_period();
    Estimate<double> phase = model[ichan]->get_phase();

    toa.set_arrival   (data->get_epoch() + phase.val * period);
    toa.set_error     (sqrt(phase.var) * period * 1e6);

    toa.set_telescope (archive->get_telescope_code());

    string aux = basename (archive->get_filename());
    toa.set_auxilliary_text (aux);

    toa.unload (tim_file);

    cerr << aux << " freq = " << freq << " chisq = " << reduced_chisq 
	 << " phase = " << phase.val << " +/- " << phase.get_error() << endl;

  }

  if (solution[ichan]) {

    float chisq = solution[ichan]->chisq(transformation[ichan]);

    if (chisq > 5.0) {

      if (verbose > 2) {

	cerr << "  BIG DIFFERENCE=" << chisq << endl;
	cerr << "    OLD\t\t\t\tNEW" << endl;

	Calibration::Instrument test;
	solution[ichan]->update(&test);
	
	unsigned nparam = test.get_nparam();
	for (unsigned ip=1; ip < nparam; ip++)
	  cerr << "  " << ip << " " << test.get_Estimate(ip)
	       << "\t\t" << transformation[ichan]->get_Estimate(ip) << endl;

      }

      solution[ichan] = 0;
      big_difference ++;

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
	transformation[ichan] = new_transformation ();
      solution[ichan]->update( transformation[ichan] );
    }
  }
}

   
//! Initialize the PolnCalibration::transformation attribute
void Pulsar::PulsarCalibrator::calculate_transformation ()
{
  cerr << "Pulsar::PulsarCalibrator::calculate_transformation" << endl;
  // if the mean solution is not required, then the last
  // transformation calculated will be returned
  if (!mean_solution)
    return;

  unsigned nchan = model.size();

  transformation.resize( nchan );
  update_solution ();
}
