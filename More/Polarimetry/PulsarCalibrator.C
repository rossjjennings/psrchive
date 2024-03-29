/***************************************************************************
 *
 *   Copyright (C) 2004-2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/StandardSpectra.h"
#include "Pulsar/PolnSpectrumStats.h"
#include "Pulsar/PolnProfileStats.h"

#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/ReferenceCalibrator.h"

#include "Pulsar/ArchiveMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"

#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/Instrument.h"
#include "Pulsar/VariableBackend.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/Feed.h"
#include "Pulsar/MeanInstrument.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/SystemCalibratorUnloader.h"
#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/Fourier.h"
#include "Pulsar/Pulsar.h"

#include "MEAL/Complex2Math.h"
#include "MEAL/GimbalLockMonitor.h"

#include "toa.h"
#include "strutil.h"

#include <assert.h>

using namespace std;
using namespace Calibration;

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type* model)
{
  if (model)
    type = model;
  else
    type = new CalibratorTypes::bri00_Eq19;

  maximum_harmonic = 0;
  chosen_maximum_harmonic = 0;
  choose_maximum_harmonic = false;

  monitor_gimbal_lock = false;

  solve_each = false;
  fixed_phase = false;
  shared_phase = false;
}

//! Constructor
Pulsar::PulsarCalibrator::~PulsarCalibrator ()
{
}

void Pulsar::PulsarCalibrator::export_prepare () const try
{
  const_cast<PulsarCalibrator*>(this)->solve_prepare ();
}
catch (Error& error)
{
  throw error += "Pulsar::PulsarCalibrator::export_prepare";
}

void Pulsar::PulsarCalibrator::set_maximum_harmonic (unsigned max)
{
  maximum_harmonic = max;
}

void Pulsar::PulsarCalibrator::set_choose_maximum_harmonic (bool flag)
{
  choose_maximum_harmonic = flag;
}

void Pulsar::PulsarCalibrator::set_fixed_phase (bool flag)
{
  fixed_phase = flag;
}

void Pulsar::PulsarCalibrator::share_phase ()
{
  shared_phase = true;
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
    warning << "Pulsar::PulsarCalibrator::set_standard '" 
	    << data->get_filename() << "' has not been calibrated" << endl;

  set_calibrator( standard = data->clone() );

  // set_calibrator sets the Receiver extension, but this should come
  // from the uncalibrated data; therefore, reset the receiver to NULL
  set_Receiver (0);

  FrontendCorrection correct;
  if (correct.required(data))
  {
    if (verbose)
      cerr << "PulsarCalibrator::set_standard correcting frontend" << endl;
    correct.calibrate( standard );
  }
  else if (verbose)
    cerr << "PulsarCalibrator::set_standard frontend correction not required"
	 << endl;

  /*
    Select the on-pulse and baseline regions
  */
  {
    const Integration* integration = standard->get_Integration (0);
    Reference::To<Integration> clone = integration->clone();
    clone->expert()->fscrunch ();

    Pulsar::PolnSpectrumStats stats;
    stats.select_profile( clone->new_PolnProfile (0) );
    stats.get_regions( onpulse, baseline );

    if (choose_maximum_harmonic)
    {
      chosen_maximum_harmonic = stats.get_last_harmonic();
      if (verbose)
	cerr << "Pulsar::PulsarCalibrator::set_standard max harmonic="
	     << chosen_maximum_harmonic << "/" << clone->get_nbin()/2 << endl;
    }
  }
}

unsigned Pulsar::PulsarCalibrator::get_nharmonic () const
{
  if (choose_maximum_harmonic)
    return chosen_maximum_harmonic;
  else if (maximum_harmonic)
    return maximum_harmonic;
  else
    return standard->get_nbin()/2;
}

//! Get the number of pulsar phase bin input polarization states
unsigned Pulsar::PulsarCalibrator::get_nstate_pulsar () const
{
  return get_nharmonic ();
}

void Pulsar::PulsarCalibrator::build (unsigned nchan) try
{
  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::build nchan=" << nchan << endl;

  transformation_resize (nchan);
  solution.resize (nchan);
  reduced_chisq.resize (nchan);

  const Integration* integration = standard->get_Integration (0);
  unsigned model_nchan = integration->get_nchan();
  if (model_nchan != 1 && model_nchan != nchan)
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::build",
		 "template nchan=%d != required nchan=%d", model_nchan, nchan);

  correct_interstellar_Faraday_rotation =
    standard->get_faraday_corrected() ||
    (model_nchan == 1 && nchan > model_nchan);

  mtm.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) try
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::build ichan=" << ichan << endl;

    set_transformation_invalid (ichan, "PulsarCalibrator::build uninitialized");
    solution[ichan] = 0;
    mtm[ichan] = 0;

    unsigned mchan = ichan;
    if (model_nchan == 1)
      mchan = 0;

    if (integration->get_weight (mchan) == 0)
    {
      if (verbose > 2)
	cerr << "Pulsar::PulsarCalibrator::build ichan="
	     << ichan << " flagged invalid" << endl;

      continue;
    }

    mtm[ichan] = new PolnProfileFit;

    if (fixed_phase)
      mtm[ichan]->remove_phase ();

    if (shared_phase)
      mtm[ichan]->share_phase ();

    // the equation transformation will be managed by the SignalPath class
    mtm[ichan]->manage_equation_transformation = false;

    mtm[ichan]->set_normalize_by_invariant (normalize_by_invariant);

    if (choose_maximum_harmonic)
      mtm[ichan]->set_maximum_harmonic( chosen_maximum_harmonic );
    else if (maximum_harmonic)
      mtm[ichan]->set_maximum_harmonic( maximum_harmonic );

    mtm[ichan]->set_regions ( onpulse, baseline );
    mtm[ichan]->set_standard ( integration->new_PolnProfile (mchan) );

  }
  catch (Error& error)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::build ichan=" << ichan 
           << " error=" << error.get_message() << " (flagged invalid)" << endl;
    mtm[ichan] = 0;
  }

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard exit" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::PulsarCalibrator::build";
}

unsigned Pulsar::PulsarCalibrator::get_nchan () const
{
  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::get_nchan" << endl;

  if (mtm.size())
    return mtm.size();

  return SystemCalibrator::get_nchan ();
}

void Pulsar::PulsarCalibrator::init_model (unsigned ichan)
{
  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::init_model" << endl;

  if (ichan >= mtm.size())
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::init_model",
		 "ichan=%u >= mtm.size()=%u", ichan, mtm.size());

  if (ichan >= model.size())
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::init_model",
		 "ichan=%u >= model.size()=%u", ichan, model.size());

  // share the measurement equations between PolnProfileFit and SignalPath
  if (mtm[ichan])
    model[ichan]->set_equation( mtm[ichan]->get_equation() );
  else
    model[ichan]->set_valid( false );

  if (normalize_by_invariant)
    model[ichan]->set_constant_pulsar_gain ();

  SystemCalibrator::init_model (ichan);
}

bool Pulsar::PulsarCalibrator::calibrator_match (const Archive* data, std::string& reason)
{
  Archive::Match match;

  if (data->get_type() == Signal::Pulsar)
    match.set_check_standard (true);

  match.set_check_calibrator (true);
  match.set_check_nbin (false);
  match.set_check_state (false);

  if (mtm.size() == 0)
  {
    // until the model is built, allow sideband mismatch
    match.set_check_bandwidth_sign (false);
  }

  bool one_channel = standard->get_nchan() == 1 && data->get_nchan() >= 1;

  if (one_channel)
  {
    match.set_check_nchan (false);
    match.set_check_centre_frequency (false);
    match.set_check_bandwidth (false);
  }

  bool result = match.match (get_calibrator(), data);
  reason = match.get_reason();

  return result;
}

//! Ensure that the pulsar observation can be added to the data set
void Pulsar::PulsarCalibrator::match (const Archive* data)
{
  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::match"
      " data->nchan=" << data->get_nchan() << endl;

  string reason;
  if (!calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::match",
                 "mismatch between calibrator\n\t"
                 + get_calibrator()->get_filename() +
                 " and\n\t" + data->get_filename() + reason);

  if (!has_Receiver())
    set_Receiver (data);

  bool one_channel = standard->get_nchan() == 1 && data->get_nchan() > 1;

  if (one_channel)
    PolnCalibrator::set_calibrator (data);

  if (mtm.size() == 0)
  {
    if (get_calibrator()->get_bandwidth() == -1.0 * data->get_bandwidth())
    {
      cerr << "Pulsar::PulsarCalibrator::match reverse channels in template" << endl;
      get_calibrator()->reverse_chan();
      get_calibrator()->set_bandwidth( data->get_bandwidth() );
    }

    build( data->get_nchan() );
  }
}

/*!
  If solve_each is set, then this method will solve for the solution of
  the specified channel
*/
void Pulsar::PulsarCalibrator::add_pulsar
( CoherencyMeasurementSet& measurements,
  const Integration* integration, unsigned ichan )
{
  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::add_pulsar start" << endl;

  setup (integration, ichan);

  assert (ichan < PolnCalibrator::get_nchan(false));

  if (!get_transformation_valid(ichan))
  {
    cerr << "Pulsar::PulsarCalibrator::add_pulsar no transformation for ichan=" << ichan << endl;
    return;
  }

  assert (ichan < mtm.size());

  if (solve_each)
  {
    /*
      if solving for each pulsar observation added,
      first delete everything added so far
    */
    mtm[ichan]->delete_observations ();
	
    unsigned nbin = integration->get_nbin();

    mtm[ichan]->set_plan
      ( FTransform::Agent::current->get_plan (nbin, FTransform::frc) );
  }

  try
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar adding to path index="
	   << measurements.get_transformation_index() << endl;

    get_data_call ++;
    mtm[ichan]->add_observation( measurements,
				 integration->new_PolnProfile (ichan) );
  }
  catch (Error& error)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar ichan=" << ichan 
	   << " error\n\t" << error << endl;
    get_data_fail ++;
    return;
  }

  if (solve_each)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar"
	" solving ichan=" << ichan << endl;

    queue.submit( this, &Pulsar::PulsarCalibrator::solve1,
		  measurements );
  }

  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::add_pulsar exit" << endl;
}

//! Ensure that all queued jobs have finished
void Pulsar::PulsarCalibrator::wait ()
{
  queue.wait ();
}

//! Add the observation to the set of constraints
void Pulsar::PulsarCalibrator::add_pulsar (const Archive* data, unsigned isub)
try
{
  SystemCalibrator::add_pulsar (data, isub);

  queue.wait ();

  if (solve_each && unload_each)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar unload solution" << endl;

    unload_each->set_filename (data, isub);
    unload_each->unload (this);

    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar store solution" << endl;

    // clear the map for each new archive
    if (isub == 0)
      store_each.clear();

    Reference::Vector<MEAL::Complex2>& store = store_each[isub];
    store.resize( get_nchan() );
    for (unsigned i=0; i < store.size(); i++)
      if (get_transformation_valid(i))
	store[i] = PolnCalibrator::get_transformation(i)->clone();
  }
}
catch (Error& error)
{
  throw error += "Pulsar::PulsarCalibrator::add_pulsar";
}

void Pulsar::PulsarCalibrator::add_calibrator (const ReferenceCalibrator* p)
{
  if (verbose > 2)
    cerr << "Pulsar::PulsarCalibrator::add_calibrator" << endl;

  const Archive* cal = p->get_Archive();

  if ( cal->get_type() != Signal::PolnCal )
  {
    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::add_calibrator ignoring "
	   << Source2string(cal->get_type()) << " observation";
    return;
  }

  SystemCalibrator::add_calibrator (p);
}

//! Return the transformation to be used for precalibration
MEAL::Complex2*
Pulsar::PulsarCalibrator::get_transformation (const Archive* data,
					      unsigned isub, unsigned ichan)
{
  if (store_each.size() == 0)
    return SystemCalibrator::get_transformation (data, isub, ichan);

  return store_each[isub][ichan];
}

class interface : public MEAL::GimbalLockMonitor
{
public:
  bool check (ReceptionModel*) { lock_detected(); return true; }
};

Functor< bool(ReceptionModel*) >
gimbal_lock( Instrument* instrument, unsigned receptor )
{
  Feed* feed = instrument->get_feed();
  SingleAxis* backend = instrument->get_backend()->get_backend();

  interface* condition = new interface;
  condition->set_yaw  ( feed->get_orientation_transformation( receptor ) );
  condition->set_pitch( feed->get_ellipticity_transformation( receptor ) );
  condition->set_roll ( backend->get_rotation_transformation() );

  return Functor< bool(ReceptionModel*) >
    ( condition, &interface::check );
}

MEAL::Complex2* Pulsar::PulsarCalibrator::new_transformation (unsigned ichan)
{
  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::new_transformation ichan="
	 << ichan << endl;

  if (ichan >= model.size())
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::new_transformation",
                 "ichan=%d > nchan=%d", ichan, model.size());

  MEAL::Complex2* xform = model[ichan]->get_transformation();

  Instrument* instrument = dynamic_cast<Instrument*> (xform);
  if (!instrument)
    return xform;

  instrument->set_cyclic();

  if (monitor_gimbal_lock)
    for (unsigned i=0; i<2; i++)
      mtm[ichan]->get_equation()->get_solver()->add_convergence_condition
	( gimbal_lock(instrument, i) );

  return instrument;
}

void Pulsar::PulsarCalibrator::setup (const Integration* data, unsigned ichan)
{
  assert (ichan < PolnCalibrator::get_nchan(false) );
  assert (ichan < mtm.size());

  if (!mtm[ichan])
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup standard ichan="
	   << ichan << " flagged invalid" << endl;
    set_transformation_invalid(ichan, "PulsarCalibrator::setup no model");
    return;
  }

  if (data->get_weight (ichan) == 0)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup observation ichan="
	   << ichan << " flagged invalid" << endl;
    set_transformation_invalid(ichan, "PulsarCalibrator::setup data have zero weigth");
    return;
  }

  if (!get_transformation_valid(ichan)) 
  {
    set_transformation(ichan, new_transformation(ichan));

    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup set ichan=" << ichan << endl;

    mtm[ichan]->set_transformation (PolnCalibrator::get_transformation(ichan));
  }
}

string get_state (MEAL::Function* f)
{
  string state;
  f->print (state);
  return state;
}

void Pulsar::PulsarCalibrator::solve1 (const CoherencyMeasurementSet& data)
{
  unsigned ichan = data.get_ichan();
  
  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::solve1 ichan=" << ichan << endl;

  Reference::To<MEAL::Function> backup;

  assert (ichan < PolnCalibrator::get_nchan(false));

  CoherencyMeasurementSet measurements (data);
  submit_pulsar_data (measurements);
      
  for (unsigned tries=0 ; tries < 2; tries ++) try
  {
    // setup (data, ichan);

    if (!get_transformation_valid(ichan))
      return;

    if (!backup)
      backup = PolnCalibrator::get_transformation(ichan)->clone();

    assert (ichan < solution.size());
    assert (ichan < reduced_chisq.size());
    assert (ichan < mtm.size());

    if (solution[ichan])
    {
      solution[ichan]->update( PolnCalibrator::get_transformation(ichan) );
      //cerr << "update gain=" << get_transformation(ichan)->get_param(0) << endl;
    }
    else if (ichan>0 && tries==0 && solution[ichan-1]
             && reduced_chisq[ichan-1] < 1.2)
    {
      solution[ichan-1]->update( PolnCalibrator::get_transformation(ichan) );
      //cerr << "copy gain=" << get_transformation(ichan)->get_param(0) << endl;
    }
    else if (tries && backup)
    {
      if (verbose)
        cerr << "Pulsar::PulsarCalibrator::solve1 backup "
             << get_state(backup) << endl;

      PolnCalibrator::get_transformation(ichan)->copy (backup);
    }

    MJD epoch = get_epoch(); 
    model[ichan]->set_reference_epoch ( epoch );

    configure( mtm[ichan]->get_equation() );

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 pre-fit " 
           << get_state(PolnCalibrator::get_transformation(ichan)) << endl;


    mtm[ichan]->solve ();

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 post-fit " 
           << get_state(PolnCalibrator::get_transformation(ichan)) << endl;

    unsigned nfree = mtm[ichan]->get_equation()->get_solver()->get_nfree ();
    float chisq = mtm[ichan]->get_equation()->get_solver()->get_chisq ();

    reduced_chisq[ichan] = chisq / nfree;

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 chisq=" << chisq 
	   << "/nfree=" << nfree << " = " << reduced_chisq[ichan] << endl;

    if (retry_chisq == 0 || reduced_chisq[ichan] < retry_chisq)
      break;

    cerr << "Pulsar::PulsarCalibrator::solve1 ichan=" << ichan
	 << " retry reduced chisq=" << reduced_chisq[ichan] << endl;

    // try again with a fresh start
    string reason = "PulsarCalibrator::solve1 reduced chisq = " + tostring(reduced_chisq[ichan]) 
	    + " above threshold=" + tostring (retry_chisq);

    set_transformation_invalid(ichan, reason);

    if (!solution[ichan])
      break;

    solution[ichan] = 0;

  }
  catch (Error& error)
  {
    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 ichan=" << ichan 
	   << " error" << error << endl;
#if 0
    if (verbose > 2)
      cerr << error << endl;
    else
      cerr << error.get_message() << endl;
#endif

    PolnCalibrator::get_transformation(ichan)->copy (backup);

    set_transformation_invalid(ichan, error.get_message());
    solution[ichan] = 0;
  }

  if (!get_transformation_valid(ichan))
    return;

#ifdef _DEBUG
  for (unsigned ip=0; ip < PolnCalibrator::get_transformation(ichan)->get_nparam(); ip++)
    cerr << "  " << ip << " " << PolnCalibrator::get_transformation(ichan)->get_Estimate(ip)
	 << endl;

#endif

  if (solution[ichan])
  {
    float chisq = solution[ichan]->chisq(PolnCalibrator::get_transformation(ichan));

    if (chisq > 5.0)
    {
      if (verbose > 2)
      {
	cerr << "  BIG DIFFERENCE=" << chisq << endl;
	cerr << "    OLD\t\t\t\tNEW" << endl;

	Instrument test;
	solution[ichan]->update(&test);
	
	unsigned nparam = test.get_nparam();
	for (unsigned ip=1; ip < nparam; ip++)
	  cerr << "  " << ip << " " << test.get_Estimate(ip)
	       << "\t\t" << PolnCalibrator::get_transformation(ichan)->get_Estimate(ip) << endl;
      }

      solution[ichan] = 0;
      big_difference ++;
    }
  }

  if ( dynamic_cast<Instrument*>( PolnCalibrator::get_transformation(ichan) ) )
  {
    if (!solution[ichan])
      solution[ichan] = new MeanInstrument;

    solution[ichan]->integrate(  PolnCalibrator::get_transformation(ichan) );
  }
}

void Pulsar::PulsarCalibrator::update_solution ()
{
  unsigned nchan = solution.size ();
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (solution[ichan])
    {
      assert (ichan < PolnCalibrator::get_nchan(false));

      if (!get_transformation_valid(ichan))
	set_transformation (ichan, new_transformation (ichan));

      solution[ichan]->update( PolnCalibrator::get_transformation(ichan) );
    }
  }
}

const Pulsar::PolnProfileFit*
Pulsar::PulsarCalibrator::get_mtm (unsigned ichan) const
{
  if (ichan >= mtm.size())
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::get_mtm",
		 "ichan=%u >= mtm.size=%u", ichan, mtm.size());

  return mtm[ichan];
}

void Pulsar::PulsarCalibrator::set_solve_each (bool flag)
{
  solve_each = flag;
}

void Pulsar::PulsarCalibrator::set_unload_each (Unloader* unloader)
{
  unload_each = unloader;
}
