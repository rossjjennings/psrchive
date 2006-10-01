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

#include "Pulsar/Instrument.h"
#include "Pulsar/MeanInstrument.h"

#include "MEAL/Complex2Math.h"

#include "toa.h"

using namespace std;

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

void Pulsar::PulsarCalibrator::set_normalize_gain (bool flag)
{
  normalize_gain = flag;
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
  
  CorrectionsCalibrator correct;
  if (correct.needs_correction(data)) {
    cerr << "Pulsar::PulsarCalibrator::set_standard correcting instrument" 
         << endl;
    correct.calibrate( clone );
  }

  if (clone->get_nchan () > 1)
    build (clone->get_nchan());

}

void Pulsar::PulsarCalibrator::build (unsigned nchan)
{
  transformation.resize (nchan);
  solution.resize (nchan);

  const Integration* integration = get_calibrator()->get_Integration (0);
  unsigned model_nchan = integration->get_nchan();

  if (model_nchan != 1 && model_nchan != nchan)
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::build",
		 "template nchan=%d != required nchan=%d", model_nchan, nchan);

  model.resize (model_nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::build ichan=" << ichan << endl;

    transformation[ichan] = 0;
    solution[ichan] = 0;

    if (ichan > 0 && model_nchan == 1)
      continue;

    if (integration->get_weight (ichan) == 0) {
      if (verbose)
	cerr << "Pulsar::PulsarCalibrator::build ichan="
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

  bool one_channel = get_calibrator()->get_nchan() == 1;

  if (one_channel) {
    match.set_check_nchan (false);
    match.set_check_centre_frequency (false);
    match.set_check_bandwidth (false);
  }

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

  if (one_channel)
    build (nchan);

  epoch = data->start_time ();

  if (tim_file)
    archive = data;

  for (unsigned isub=0; isub<nsub; isub++) {

    // cerr << "solving isub=" << isub << " ..." << endl;

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
  unsigned mchan = ichan;

  bool one_channel = get_calibrator()->get_nchan() == 1;

  if (one_channel)
    mchan = 0;

  if (!model[mchan]) {
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

    bool set_model = false;

    if (!transformation[ichan]) {
      transformation[ichan] = new_transformation();
      set_model = true;
    }

    if (one_channel || set_model)
      model[mchan]->set_transformation (transformation[ichan] * &corrections);

    if (solution[ichan])
      solution[ichan]->update( transformation[ichan] );
    else if (ichan>0 && tries==0 && solution[ichan-1])
      solution[ichan-1]->update( transformation[ichan] );

    model[mchan]->fit( data->new_PolnProfile (ichan) );

    unsigned iterations = model[mchan]->get_fit_iterations ();
    unsigned nfree = model[mchan]->get_fit_nfree ();
    float chisq = model[mchan]->get_fit_chisq ();

    reduced_chisq = chisq / nfree;

    string out;
    transformation[ichan]->print (out);

    if (reduced_chisq < 2.0)
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
    Estimate<double> phase = model[mchan]->get_phase();

    toa.set_arrival   (data->get_epoch() + phase.val * period);
    toa.set_error     (sqrt(phase.var) * period * 1e6);

    toa.set_telescope (archive->get_telescope_code());

    string aux = basename (archive->get_filename().c_str());
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

  if (normalize_gain) {
    Calibration::Instrument* inst;
    inst = dynamic_cast<Calibration::Instrument*>(transformation[ichan].get());
    if (!inst)
      throw Error (InvalidState, "Pulsar::PulsarCalibrator::solve",
		   "transformation[%d] is not an Instrument", ichan);
    inst->set_gain(1.0);
  }

  solution[ichan]->integrate( transformation[ichan] );
  
}

//! Set the flag to return the mean solution or the last fit
void Pulsar::PulsarCalibrator::set_return_mean_solution (bool return_mean)
{
  mean_solution = return_mean;
}

void Pulsar::PulsarCalibrator::update_solution ()
{
  unsigned nchan = solution.size ();
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
  // if the mean solution is not required, then the last
  // transformation calculated will be returned

  if (!mean_solution)
    return;

  unsigned nchan = solution.size();

  transformation.resize( nchan );
  update_solution ();
}
