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
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/PolnProfileFit.h"

#include "Pulsar/Instrument.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/Feed.h"
#include "Pulsar/MeanInstrument.h"

#include "Pulsar/ReceptionModel.h"
#include "Pulsar/Fourier.h"

#include "MEAL/Complex2Math.h"
#include "MEAL/Complex2Value.h"
#include "MEAL/GimbalLockMonitor.h"

#include "toa.h"
#include "strutil.h"

using namespace std;

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type model)
{
  model_type = model;
  maximum_harmonic = 0;
  chosen_maximum_harmonic = 0;
  choose_maximum_harmonic = false;
  mean_solution = true;
  monitor_gimbal_lock = true;
  solve_each = true;
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
  throw error += "Pulsar::PulsarCalibrator::get_epoch";
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
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::set_standard correcting instrument" 
	   << endl;
    correct.calibrate( clone );
  }

  if (choose_maximum_harmonic) {

    const Integration* integration = get_calibrator()->get_Integration (0);

    Reference::To<Integration> clone = integration->clone();
    clone->expert()->fscrunch ();

    PolnProfileFit temp;

    if (verbose > 2)
      PolnProfileFit::verbose = true;

    temp.choose_maximum_harmonic = true;
    temp.set_standard ( clone->new_PolnProfile (0) );

    chosen_maximum_harmonic = temp.get_nharmonic();

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::build max harmonic="
           << chosen_maximum_harmonic << "/" << clone->get_nbin()/2 << endl;

  }

  if (clone->get_nchan () > 1)
    build (clone->get_nchan());

}

unsigned Pulsar::PulsarCalibrator::get_nharmonic () const
{
  if (choose_maximum_harmonic)
    return chosen_maximum_harmonic;
  else if (maximum_harmonic)
    return maximum_harmonic;
  else
    return get_calibrator()->get_nbin()/2;
}

void Pulsar::PulsarCalibrator::build (unsigned nchan)
{
  transformation.resize (nchan);
  solution.resize (nchan);
  phase_shift.resize (nchan);
  reduced_chisq.resize (nchan);

  const Integration* integration = get_calibrator()->get_Integration (0);
  unsigned model_nchan = integration->get_nchan();
  if (model_nchan != 1 && model_nchan != nchan)
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::build",
		 "template nchan=%d != required nchan=%d", model_nchan, nchan);

  model.resize (model_nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::build ichan=" << ichan << endl;

    transformation[ichan] = 0;
    solution[ichan] = 0;

    if (ichan > 0 && model_nchan == 1)
      continue;

    if (integration->get_weight (ichan) == 0) {
      if (verbose > 2)
	cerr << "Pulsar::PulsarCalibrator::build ichan="
	     << ichan << " flagged invalid" << endl;
      continue;
    }

    model[ichan] = new PolnProfileFit;

    if (choose_maximum_harmonic)
      model[ichan]->set_maximum_harmonic( chosen_maximum_harmonic );
    else if (maximum_harmonic)
      model[ichan]->set_maximum_harmonic( maximum_harmonic );

    model[ichan]->set_standard ( integration->new_PolnProfile (ichan) );

  }

  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::set_standard exit" << endl;

}

void Pulsar::PulsarCalibrator::set_nthread (unsigned nthread)
{
  queue.resize (nthread);
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

  if (one_channel && !solve_each) 
  {
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
      "  data have already been calibrated" << endl;

  Reference::To<Archive> clone;

  if (must_correct_backend (data) )
  {
    clone = data->clone();
    correct_backend (clone);
    data = clone;
  }

  CorrectionsCalibrator correct;

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();
  unsigned nbin = data->get_nbin();

  if (one_channel)
    build (nchan);

  epoch = data->start_time ();

  if (tim_file)
    archive = data;

  for (unsigned isub=0; isub<nsub; isub++)
  {

    // cerr << "solving isub=" << isub << " ..." << endl;

    const Integration* integration = data->get_Integration (isub);

    corrections = correct.get_transformation( data, isub );

    // if 5% of the solutions diverge from the mean, clear the mean
    unsigned clean_mean = nchan/20;
    big_difference = 0;

    for (unsigned ichan=0; ichan<nchan; ichan++) try
    {
      unsigned mchan = ichan;
      if (one_channel)
	mchan = 0;

      if (!model[mchan])
	continue;

      if (!solve_each)
      {
	add_observation (integration, ichan);
	continue;
      }
	  
      model[mchan]->set_plan
	( FTransform::Agent::current->get_plan (nbin, FTransform::frc) );

      queue.submit( this, &Pulsar::PulsarCalibrator::solve1,
		    integration, ichan );

      // the current mean is no longer providing a good first guess; clear it!
      if (big_difference >= clean_mean)
      {
	cerr << "Pulsar::PulsarCalibrator::add_observation"
	  " clearing the current mean" << endl;

	queue.wait ();

	for (unsigned jchan=ichan+1; jchan<nchan; jchan++)
	  solution[jchan] = 0;
	big_difference = 0;
      }

    }
    catch (Error& error)
    {
      cerr << "Pulsar::PulsarCalibrator::add_observation ichan=" << ichan
	   << error << endl;
    }

    queue.wait ();

    if (!tim_file)
      continue;

    // produce TOAs!

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!transformation[ichan])
        continue;

      Tempo::toa toa (Tempo::toa::Parkes);

      double freq = integration->get_centre_frequency (ichan);
      toa.set_frequency (freq);

      double period = integration->get_folding_period();
      Estimate<double> phase = phase_shift[ichan];

      toa.set_arrival   (integration->get_epoch() + phase.val * period);
      toa.set_error     (sqrt(phase.var) * period * 1e6);

      toa.set_telescope (archive->get_telescope());
      
      string aux = basename (archive->get_filename());
      toa.set_auxilliary_text (aux);

      toa.unload (tim_file);
      
      if (verbose > 2)
	cerr << aux << " freq=" << freq << " chisq=" << reduced_chisq[ichan]
	     << " phase=" << phase.val << " +/- " << phase.get_error() << endl;
    }

  }

  built = false;

}
catch (Error& error)
{
  throw error += "Pulsar::PulsarCalibrator::add_observation";
}

class interface : public MEAL::GimbalLockMonitor
{
public:
  bool check (Calibration::ReceptionModel*) { lock_detected(); return true; }
};

Functor< bool(Calibration::ReceptionModel*) >
gimbal_lock( Calibration::Instrument* instrument, unsigned receptor )
{
  Calibration::Feed* feed = instrument->get_feed();
  Calibration::SingleAxis* backend = instrument->get_backend();

  interface* condition = new interface;
  condition->set_yaw  ( feed->get_orientation_transformation( receptor ) );
  condition->set_pitch( feed->get_ellipticity_transformation( receptor ) );
  condition->set_roll ( backend->get_rotation_transformation() );

  return Functor< bool(Calibration::ReceptionModel*) >
    ( condition, &interface::check );
}

MEAL::Complex2* Pulsar::PulsarCalibrator::new_transformation (unsigned ichan)
{
  Calibration::Instrument* instrument = new Calibration::Instrument;
  instrument->set_cyclic();

  if (monitor_gimbal_lock)
    for (unsigned i=0; i<2; i++)
      model[ichan]->get_model()->add_convergence_condition
	( gimbal_lock(instrument, i) );

  return instrument;
}

unsigned
Pulsar::PulsarCalibrator::setup (const Integration* data, unsigned ichan)
{
  unsigned mchan = ichan;

  bool one_channel = get_calibrator()->get_nchan() == 1;

  if (one_channel)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup apply single channel to ichan="
	   << ichan << " " << endl;
    mchan = 0;
  }

  if (!model[mchan])
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup standard ichan="
	   << ichan << " flagged invalid" << endl;
    transformation[ichan] = 0;
    return 0;
  }

  if (data->get_weight (ichan) == 0)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup observation ichan="
	   << ichan << " flagged invalid" << endl;
    transformation[ichan] = 0;
    return 0;
  }

  bool set_model = false;

  if (!transformation[ichan])
  {
    transformation[ichan] = new_transformation(mchan);
    set_model = true;
  }

  if (one_channel || set_model) 
  {
    Reference::To<MEAL::Complex2> c = new MEAL::Complex2Value(corrections);
    model[mchan]->set_transformation (transformation[ichan] * c);
  }

  return mchan;
}

//! Add data to the measurement equation for the given channel
void Pulsar::PulsarCalibrator::add_observation (const Integration* data,
						unsigned ichan)
{
  setup (data, ichan);

  if (!transformation[ichan])
    return;

  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::add_observation chan=" << ichan << endl;

  model[ichan]->add_observation( data->new_PolnProfile (ichan) );
}

void Pulsar::PulsarCalibrator::solve ()
{
  unsigned nchan = solution.size ();

  solved_transformation.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    if (!model[ichan])
      continue;

    cerr << "Pulsar::PulsarCalibrator::solve chan=" << ichan << endl;
    
    model[ichan]->solve ();

    unsigned nfree = model[ichan]->get_model()->get_fit_nfree ();
    float chisq = model[ichan]->get_model()->get_fit_chisq ();
      
    reduced_chisq[ichan] = chisq / nfree;
    
    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve chisq=" << chisq 
	   << "/nfree=" << nfree << " = " << reduced_chisq[ichan] << endl;

    solved_transformation[ichan] = transformation[ichan];
  }
  catch (Error& error)
  {
    cerr << error << endl;
  }

  // ensure that calculate_transformation is called again
  transformation.resize (0);
}

void 
Pulsar::PulsarCalibrator::solve1 (const Integration* data, unsigned ichan)
{
  for (unsigned tries=0 ; tries < 2; tries ++) try
  {
    unsigned mchan = setup (data, ichan);

    if (!transformation[ichan])
      return;

    if (solution[ichan])
      solution[ichan]->update( transformation[ichan] );

    else if (ichan>0 && tries==0 && solution[ichan-1]
             && reduced_chisq[ichan-1] < 1.2)
      solution[ichan-1]->update( transformation[ichan] );

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 chan=" << mchan << endl;

    model[mchan]->fit( data->new_PolnProfile (ichan) );

    unsigned nfree = model[mchan]->get_model()->get_fit_nfree ();
    float chisq = model[mchan]->get_model()->get_fit_chisq ();

    reduced_chisq[ichan] = chisq / nfree;

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 chisq=" << chisq 
	   << "/nfree=" << nfree << " = " << reduced_chisq[ichan] << endl;

    phase_shift[ichan] = model[mchan]->get_phase();

    if (reduced_chisq[ichan] < 2.0)
      break;

    cerr << "Pulsar::PulsarCalibrator::solve1 ichan=" << ichan
	 << " invalid reduced chisq=" << reduced_chisq[ichan] << endl;

    // try again with a fresh start
    transformation[ichan] = 0;

    if (!solution[ichan])
      break;

    solution[ichan] = 0;

  }
  catch (Error& error) {
    cerr << "Pulsar::PulsarCalibrator::solve1 ichan=" << ichan 
         << " error" << error << endl;
#if 0
    if (verbose > 2)
      cerr << error << endl;
    else
      cerr << error.get_message() << endl;
#endif
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
    inst = dynamic_kast<Calibration::Instrument>(transformation[ichan]);
    if (!inst)
      throw Error (InvalidState, "Pulsar::PulsarCalibrator::solve1",
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
	transformation[ichan] = new_transformation (ichan);
      solution[ichan]->update( transformation[ichan] );
    }
  }
}


//! Initialize the PolnCalibration::transformation attribute
void Pulsar::PulsarCalibrator::calculate_transformation ()
{
  cerr << "Pulsar::PulsarCalibrator::calculate_transformation" << endl;

  unsigned nchan = solution.size();

  if (!solve_each)
  {
    if (solved_transformation.size() != nchan)
      throw Error (InvalidState,
		   "Pulsar::PulsarCalibrator::calculate_transformation",
		   "measurement equations have not yet been solved");

    transformation = solved_transformation;
    return;
  }

  // if the mean solution is not required, then the last
  // transformation calculated will be returned

  if (!mean_solution)
    return;

  transformation.resize( nchan );
  update_solution ();
}

const Pulsar::PolnProfileFit*
Pulsar::PulsarCalibrator::get_model (unsigned ichan) const
{
  return model[ichan];
}

void Pulsar::PulsarCalibrator::set_solve_each (bool flag)
{
  solve_each = flag;
}

