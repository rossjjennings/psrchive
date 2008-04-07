/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/StandardSpectra.h"
#include "Pulsar/PolnSpectrumStats.h"
#include "Pulsar/PolnProfileStats.h"

#include "Pulsar/CorrectionsCalibrator.h"

#include "Pulsar/ArchiveMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"

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
using namespace Calibration;

//! Constructor
Pulsar::PulsarCalibrator::PulsarCalibrator (Calibrator::Type model)
{
  model_type = model;

  maximum_harmonic = 0;
  chosen_maximum_harmonic = 0;
  choose_maximum_harmonic = false;

  normalize_by_invariant = false;
  monitor_gimbal_lock = false;
  solve_each = false;

  tim_file = 0;
  archive = 0;
}

//! Constructor
Pulsar::PulsarCalibrator::~PulsarCalibrator ()
{
  if (tim_file)
    fclose (tim_file);
}

void Pulsar::PulsarCalibrator::export_prepare () const try
{
  const_cast<PulsarCalibrator*>(this)->solve_prepare ();
}
 catch (Error& error)
   {
     cerr << error << endl;
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

void Pulsar::PulsarCalibrator::set_normalize_by_invariant (bool flag)
{
  normalize_by_invariant = flag;
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

  set_calibrator( clone = data->clone() );
  
  CorrectionsCalibrator correct;
  if (correct.needs_correction(data))
  {
    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::set_standard correcting instrument" 
	   << endl;
    correct.calibrate( clone );
  }

  /*
    Select the on-pulse and baseline regions
  */
  {
    const Integration* integration = get_calibrator()->get_Integration (0);
    Reference::To<Integration> clone = integration->clone();
    clone->expert()->fscrunch ();

    Pulsar::PolnSpectrumStats stats;
    stats.select_profile( clone->new_PolnProfile (0) );
    stats.get_regions( on_pulse, baseline );

    if (choose_maximum_harmonic)
    {
      chosen_maximum_harmonic = stats.get_last_harmonic();
      if (verbose)
	cerr << "Pulsar::PulsarCalibrator::set_standard max harmonic="
	     << chosen_maximum_harmonic << "/" << clone->get_nbin()/2 << endl;
    }
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

void Pulsar::PulsarCalibrator::build (unsigned nchan) try
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

  mtm.resize (model_nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::build ichan=" << ichan << endl;

    transformation[ichan] = 0;
    solution[ichan] = 0;

    if (ichan > 0 && model_nchan == 1)
      continue;

    if (integration->get_weight (ichan) == 0)
    {
      if (verbose > 2)
	cerr << "Pulsar::PulsarCalibrator::build ichan="
	     << ichan << " flagged invalid" << endl;
      continue;
    }

    mtm[ichan] = new PolnProfileFit;

    // the equation transformation will be managed by the StandardModel class
    mtm[ichan]->manage_equation_transformation = false;

    mtm[ichan]->set_normalize_by_invariant (normalize_by_invariant);

    if (choose_maximum_harmonic)
      mtm[ichan]->set_maximum_harmonic( chosen_maximum_harmonic );
    else if (maximum_harmonic)
      mtm[ichan]->set_maximum_harmonic( maximum_harmonic );

    mtm[ichan]->set_regions ( on_pulse, baseline );
    mtm[ichan]->set_standard ( integration->new_PolnProfile (ichan) );

  }

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard create model" << endl;

  // create the StandardModel array of the SystemCalibrator base class
  create_model();

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard"
      " set StandardModel equations" << endl;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    // share the measurement equations between PolnProfileFit and StandardModel
    if (mtm[ichan])
      model[ichan]->set_equation( mtm[ichan]->get_equation() );
    else
      model[ichan]->valid = false;

    if (normalize_by_invariant)
      model[ichan]->set_constant_pulsar_gain ();
  }

  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::set_standard exit" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::PulsarCalibrator::build";
}

//! Ensure that the pulsar observation can be added to the data set
void Pulsar::PulsarCalibrator::match (const Archive* data)
{
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
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::match",
                 "mismatch between calibrator\n\t"
                 + get_calibrator()->get_filename() +
                 " and\n\t" + data->get_filename() + match.get_reason());

  if (one_channel)
    build (data->get_nchan());

  if (tim_file)
    archive = data;
}

/*!
  If solve_each is set, then this method will solve for the solution of
  the specified channel
*/
void Pulsar::PulsarCalibrator::add_pulsar
( Calibration::CoherencyMeasurementSet& measurements,
  const Integration* integration, unsigned ichan )
{
  if (solve_each)
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar solving each" << endl;

    bool one_channel = get_calibrator()->get_nchan() == 1;
    unsigned mchan = ichan;
    if (one_channel)
      mchan = 0;

    unsigned nbin = integration->get_nbin();

    mtm[mchan]->set_plan
      ( FTransform::Agent::current->get_plan (nbin, FTransform::frc) );

    queue.submit( this, &Pulsar::PulsarCalibrator::solve1,
		  integration, ichan );
  }
  else
  {
    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar set up" << endl;

    setup (integration, ichan);

    if (!transformation[ichan])
      return;

    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::add_pulsar adding to path index="
	   << measurements.get_transformation_index() << endl;

    mtm[ichan]->set_measurement_set( measurements );
    mtm[ichan]->add_observation( integration->new_PolnProfile (ichan) );
  }

}

//! Add the observation to the set of constraints
void Pulsar::PulsarCalibrator::add_pulsar (const Archive* data, unsigned isub)
try
{
  SystemCalibrator::add_pulsar (data, isub);

  queue.wait ();

  if (!tim_file)
    return;

  // produce TOAs

  const Integration* integration = data->get_Integration (isub);
  unsigned nchan = integration->get_nchan();

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
catch (Error& error)
{
  throw error += "Pulsar::PulsarCalibrator::add_pulsar";
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
  if (verbose)
    cerr << "Pulsar::PulsarCalibrator::new_transformation ichan="
	 << ichan << endl;

  if (ichan >= model.size())
    throw Error (InvalidState, "Pulsar::PulsarCalibrator::new_transformation",
                 "ichan=%d > nchan=%d", ichan, model.size());

  MEAL::Complex2* xform = model[ichan]->get_transformation();

  Calibration::Instrument* instrument = dynamic_cast<Instrument*> (xform);
  if (!instrument)
    return xform;

  instrument->set_cyclic();

  if (monitor_gimbal_lock)
    for (unsigned i=0; i<2; i++)
      mtm[ichan]->get_equation()->add_convergence_condition
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

  if (!mtm[mchan])
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

  if (one_channel || !transformation[ichan]) 
  {
    if (!transformation[ichan])
      transformation[ichan] = new_transformation(mchan);

    if (verbose > 2)
      cerr << "Pulsar::PulsarCalibrator::setup set mchan=" << mchan << endl;
    mtm[mchan]->set_transformation (transformation[ichan]);
  }

  return mchan;
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

    mtm[mchan]->fit( data->new_PolnProfile (ichan) );

    unsigned nfree = mtm[mchan]->get_equation()->get_fit_nfree ();
    float chisq = mtm[mchan]->get_equation()->get_fit_chisq ();

    reduced_chisq[ichan] = chisq / nfree;

    if (verbose)
      cerr << "Pulsar::PulsarCalibrator::solve1 chisq=" << chisq 
	   << "/nfree=" << nfree << " = " << reduced_chisq[ichan] << endl;

    phase_shift[ichan] = mtm[mchan]->get_phase();

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

  solution[ichan]->integrate( transformation[ichan] );
  
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

const Pulsar::PolnProfileFit*
Pulsar::PulsarCalibrator::get_mtm (unsigned ichan) const
{
  return mtm[ichan];
}

void Pulsar::PulsarCalibrator::set_solve_each (bool flag)
{
  solve_each = flag;
}

