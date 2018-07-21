/***************************************************************************
 *
 *   Copyright (C) 2003-2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/ReceptionModelReport.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/SourceInfo.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/PolnProfile.h"

// TODO: Report::factory that constructs by name
#include "Pulsar/FitGoodnessReport.h"

#include "MEAL/PhysicalCoherency.h"
#include "MEAL/Complex2Constant.h"
#include "MEAL/ProductRule.h"
#include "MEAL/Gain.h"

#include "MEAL/Tracer.h"

#include "Pauli.h"

#include <algorithm>
#include <assert.h>

using namespace std;
using Pulsar::ReceptionCalibrator;

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
ReceptionCalibrator::ReceptionCalibrator (Calibrator::Type* _type)
{
  type = _type;

  measure_cal_V = true;
  measure_cal_Q = false;
  equal_ellipticities = false;

  normalize_by_invariant = false;
  independent_gains = false;

  step_after_cal = false;

  multiple_flux_calibrators = false;

  check_pointing = false;
  physical_coherency = false;

  output_report = false;
  
  unique = 0;

  nthread = 1;
}

ReceptionCalibrator::~ReceptionCalibrator()
{
}

void ReceptionCalibrator::set_standard_data (const Archive* data)
{
  Reference::To<Archive> clone = data->clone ();

  clone->fscrunch();
  clone->tscrunch();

  Reference::To<PolnProfile> p = clone->get_Integration(0)->new_PolnProfile(0);

  standard_data = new Calibration::StandardData;

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::set_standard_data"
      " normalize_by_invariant=" << normalize_by_invariant << endl;

  standard_data->set_normalize (normalize_by_invariant);
  standard_data->select_profile( p );

  ensure_consistent_onpulse ();
}

void ReceptionCalibrator::ensure_consistent_onpulse ()
{
  if (!standard_data)
    return;

  ProfileStats* stats = standard_data->get_poln_stats()->get_stats();

  for (unsigned istate=0; istate < phase_bins.size(); istate++)
    stats->set_onpulse (phase_bins[istate], true);
}

const Pulsar::PhaseWeight* ReceptionCalibrator::get_baseline () const
{
  return standard_data->get_poln_stats()->get_stats()->get_baseline();
}

//! Get the on-pulse mask
const Pulsar::PhaseWeight* ReceptionCalibrator::get_onpulse () const
{
  return standard_data->get_poln_stats()->get_stats()->get_onpulse();
}

void ReceptionCalibrator::set_normalize_by_invariant (bool set)
{
  normalize_by_invariant = set;
  if (standard_data)
    standard_data->set_normalize (normalize_by_invariant);
}

void ReceptionCalibrator::set_step_after_cal (bool _after)
{
  step_after_cal = _after;
}

/*!
  This method is called on the first call to add_observation.
  It initializes various arrays and internal book-keeping attributes.
*/

void ReceptionCalibrator::initial_observation (const Archive* data)
{
  if (!data)
    throw Error (InvalidState, "ReceptionCalibrator::initial_observation",
		 "no Archive");

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::initial_observation" << endl;

  if (data->get_type() != Signal::Pulsar)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() 
		 + "' not a Pulsar observation");

  if (data->get_state() != Signal::Stokes)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='%s' state=%s != Signal::Stokes",
		 data->get_filename().c_str(),
		 Signal::state_string(data->get_state()));

  // use the FrontendCorrection class to determine applicability
  FrontendCorrection corrections;

  if (! corrections.required (data))
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has been corrected for parallactic angle rotation");
		 
  if (!data->get_dedispersed ())
    cerr << "Pulsar::ReceptionCalibrator WARNING archive not dedispersed\n"
      "  Pulse phase will vary as a function of frequency channel" << endl;

  set_calibrator( data->clone() );

  if (!standard_data)
    set_standard_data (data);

  Signal::Basis basis = get_calibrator()->get_basis ();

  if (basis == Signal::Circular)
  {
    if (measure_cal_Q)
    {
      cerr << "Pulsar::ReceptionCalibrator cannot measure CAL Q"
              " in circular basis" << endl;
      measure_cal_Q = false;
    }
    if (measure_cal_V)
    {
      cerr << "Pulsar::ReceptionCalibrator cannot measure CAL V"
              " in circular basis" << endl;
      measure_cal_V = false;
    }
  }

  create_model ();

  if (calibrator_estimate.size() == 0)
  {
    has_pulsar = true;
    load_calibrators ();
  }

  assert( pulsar.size() == phase_bins.size() );

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    init_estimates ( pulsar[istate], phase_bins[istate] );

  add_epoch( data->start_time () );
}

void ReceptionCalibrator::init_model (unsigned ichan)
{
  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::init_model ichan=" << ichan << endl;

  SystemCalibrator::init_model (ichan);

  if (normalize_by_invariant)
    model[ichan] -> set_constant_pulsar_gain ();

  if (step_after_cal)
    model[ichan] -> set_step_after_cal();

  if (measure_cal_Q)
    model[ichan] -> fix_orientation ();
}

void ReceptionCalibrator::load_calibrators ()
{
  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::load_calibrators" << endl;

  if (calibrator_filenames.size() == 0)
    return;

  SystemCalibrator::load_calibrators ();

  if (!fluxcal.size())
    return;

  const unsigned nchan = get_nchan();
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (fluxcal.at(ichan)->is_constrained())
      continue;

    if (!model[ichan]->get_valid())
      continue;

    cerr << "ichan=" << ichan << " flux calibrator is not constrained" << endl;
    model[ichan]->set_valid (false, "no flux cal data");
  }
}


//! Add the specified pulse phase bin to the set of state constraints
void ReceptionCalibrator::add_state (unsigned phase_bin)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_state", false);

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" 
	 << phase_bin << endl;

  for (unsigned istate=0; istate<phase_bins.size(); istate++)
    if (phase_bins[istate] == phase_bin)
    {
      cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << phase_bin
	   << " already in use" << endl;
      return;
    }

  phase_bins.push_back (phase_bin);

  pulsar.resize( pulsar.size() + 1 );

  if (has_calibrator())
    init_estimates( pulsar.back(), phase_bin );
}


//! Get the number of pulsar phase bin input polarization states
unsigned ReceptionCalibrator::get_nstate_pulsar () const
{
  return pulsar.size();
}

//! Add the specified pulsar observation to the set of constraints
void ReceptionCalibrator::add_calibrator (const Archive* data)
{
  if (data->get_type() == Signal::Calibrator)
    set_previous (data);
  else
    SystemCalibrator::add_calibrator (data);
}

bool equal_pi (const Angle& a, const Angle& b, float tolerance = 0.01);


void ReceptionCalibrator::set_previous (const Archive* data)
{
  const PolnCalibratorExtension* ext = data->get<PolnCalibratorExtension>();
  if (ext->get_type() == get_type())
  {
    cerr << "Pulsar::ReceptionCalibrator::set_previous solution of same type"
	 << endl;
    previous = new PolnCalibrator (data);
    previous_cal = data->get<CalibratorStokes>();
  }
}


//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::match (const Archive* data)
{
  check_ready ("Pulsar::ReceptionCalibrator::match", false);

  if (!has_calibrator())
    initial_observation (data);

  SystemCalibrator::match (data);
}


void ReceptionCalibrator::add_pulsar
( Calibration::CoherencyMeasurementSet& measurements,
  const Integration* integration, unsigned ichan )
{
  standard_data->set_profile( integration->new_PolnProfile (ichan) );

  for (unsigned istate=0; istate < pulsar.size(); istate++)
    add_data (measurements, pulsar.at(istate).at(ichan), ichan);

  DEBUG("Pulsar::ReceptionCalibrator::add_pulsar ADD DATA ichan=" << ichan);

  model[ichan]->get_equation()->add_data (measurements);
}

void
ReceptionCalibrator::add_data
( vector<Calibration::CoherencyMeasurement>& bins,
  Calibration::SourceEstimate& estimate,
  unsigned ichan )
{
  estimate.add_data_attempts ++;
  get_data_call ++;

  unsigned ibin = estimate.phase_bin;

  try {

    Stokes< Estimate<double> > stokes = standard_data->get_stokes( ibin );

    // NOTE: the measured states are not corrected
    Calibration::CoherencyMeasurement state (estimate.input_index);
    state.set_stokes( stokes );
    bins.push_back ( state );

    /* Correct the stokes parameters using the current best estimate of
       the instrument and the parallactic angle rotation before adding
       them to best estimate of the input state */
    
    Jones< Estimate<double> > correct;

    model[ichan]->get_equation()->set_transformation_index
      (model[ichan]->get_pulsar_path());

    correct = inv( model[ichan]->get_pulsar_transformation()->evaluate() );

    stokes = transform( stokes, correct );
    
    estimate.estimate.integrate( stokes );

  }
  catch (Error& error)
  {
    if (verbose > 2)
      cerr << "Pulsar::ReceptionCalibrator::add_data ichan=" << ichan 
	   << " ibin=" << ibin << " error\n\t" << error.get_message() << endl;
    estimate.add_data_failures ++;
    get_data_fail ++;
  }
}


void ReceptionCalibrator::prepare_calibrator_estimate (Signal::Source source)
{
  SystemCalibrator::prepare_calibrator_estimate (source);

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::prepare_calibrator_estimate" << endl;

  if (source != Signal::FluxCalOn && source != Signal::FluxCalOff)
    return;

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::prepare_calibrator_estimate"
      " FluxCalOn nchan=" << get_nchan() << endl;

  const unsigned nchan = get_nchan();

  if (fluxcal.size() == 0)
  {
    fluxcal.resize( nchan );

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ichan]->get_valid())
	continue;

      fluxcal[ichan] = new Calibration::FluxCalManager( model[ichan] );

      fluxcal[ichan]->multiple_source_states = multiple_flux_calibrators;
    }
  }

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->get_valid())
      fluxcal[ichan] = 0;

    if (!fluxcal[ichan])
      continue;

    fluxcal[ichan]->add_observation (source);
  }
}

void ReceptionCalibrator::setup_calibrators ()
{
  for (unsigned ichan=0; ichan<calibrator_estimate.size(); ichan++)
    setup_poln_calibrator (calibrator_estimate[ichan]);

  for (unsigned ichan=0; ichan<fluxcal.size(); ichan++)
    if (fluxcal[ichan])
      setup_flux_calibrator (fluxcal[ichan]);
}


/*
  For each of Stokes Q, U & V
  If the calibrator polarization vector component is not a free parameter,
  then it should be fixed to a specified value
*/

void set_fixed_QUV ( Calibration::SourceEstimate& cal, double value )
{
  for (unsigned ipol=1; ipol < 4; ipol++)
    if (!cal.source->get_infit (ipol))
      cal.source->set_Estimate (ipol, 0.0);
}

void ReceptionCalibrator::setup_poln_calibrator (Calibration::SourceEstimate& est)
{
  Signal::Basis basis = get_calibrator()->get_basis ();

  for (unsigned istokes=0; istokes<4; istokes++)
    est.source->set_infit (istokes, false);

  // calibrator flux is unity by definition
  est.source->set_Estimate (0, 1.0);
    
  if (basis == Signal::Linear)
  {
    // degree of polarization (Stokes U) may vary
    est.source->set_infit (2, true);
    
    if (measure_cal_Q)
      est.source->set_infit (1, true);
  }
  else
  {
    // degree of polarization (Stokes Q) may vary
    est.source->set_infit (1, true);
  }
  
  if (measure_cal_V && (equal_ellipticities || has_fluxcal()))
  {
    if (verbose)
      cerr << "Stokes V of the calibrator may vary" << endl;
    est.source->set_infit (3, true);
  }

  set_fixed_QUV (est, 0.0);
}

void ReceptionCalibrator::setup_flux_calibrator (Calibration::FluxCalManager*)
{
  Signal::Basis basis = get_calibrator()->get_basis ();
  
  // TODO: tell FluxCalManager to allow Stokes V to vary

  if (basis == Signal::Circular || measure_cal_V)
  {
    if (verbose)
      cerr << "ReceptionCalibrator::setup_flux_calibrator ok" << endl;
  }
  else
    throw Error (InvalidState, "ReceptionCalibrator::setup_flux_calibrator",
		 "NOT IMPLEMENTED");
}

bool ReceptionCalibrator::has_fluxcal () const
{
  return fluxcal.size() != 0;
}

const Calibration::FluxCalManager*
ReceptionCalibrator::get_fluxcal (unsigned ichan) const
{
  return fluxcal.at(ichan);
}

void ReceptionCalibrator::submit_calibrator_data 
(
 Calibration::CoherencyMeasurementSet& measurements,
 const Calibration::SourceObservation& data
 )
{
  if (data.source != Signal::FluxCalOn)
  {
    SystemCalibrator::submit_calibrator_data (measurements, data);
    return;
  }

  if (fluxcal[data.ichan])
    fluxcal[data.ichan]->submit (measurements, data);
}



void ReceptionCalibrator::integrate_calibrator_data
(
 const Jones< Estimate<double> >& correct,
 const Calibration::SourceObservation& data
 )
{
  Jones< Estimate<double> > use;
  if (previous)
    use = previous->get_response (data.ichan);
  else
    use = correct;

  if (data.source == Signal::FluxCalOn || data.source == Signal::FluxCalOff)
  {
    if (fluxcal[data.ichan])
      fluxcal[data.ichan]->integrate (use, data);
  }

  SystemCalibrator::integrate_calibrator_data (use, data);
}

void ReceptionCalibrator::integrate_calibrator_solution
(
 Signal::Source source,
 unsigned ichan,
 const MEAL::Complex2* xform
)
{
  if (source == Signal::FluxCalOn || source == Signal::FluxCalOff)
  {
    if (fluxcal[ichan])
      fluxcal[ichan]->integrate (source, xform);
  }
  else
    SystemCalibrator::integrate_calibrator_solution (source, ichan, xform);
}


//! Add the ReferenceCalibrator observation to the set of constraints
void 
ReceptionCalibrator::add_calibrator (const ReferenceCalibrator* p) try 
{
  check_ready ("Pulsar::ReceptionCalibrator::add_calibrator");

  SystemCalibrator::add_calibrator (p);
}
catch (Error& error)
{
  throw error += "Pulsar::ReceptionCalibrator::add_calibrator";
}

void ReceptionCalibrator::solve ()
{
  if (!get_prepared())
    check_ready ("Pulsar::ReceptionCalibrator::solve");

  initialize ();
  SystemCalibrator::solve ();
}

void ReceptionCalibrator::export_prepare () const
{
  const_cast<ReceptionCalibrator*>(this)->initialize();
}

void ReceptionCalibrator::initialize ()
{
  if (calibrator_estimate.size() == 0)
  {
    cerr <<
      "Pulsar::ReceptionCalibrator::initialize WARNING: \n\t"
      "Without a ReferenceCalibrator observation, \n\t"
      "there remains a degeneracy along the Stokes V axis and \n\t"
      "an unconstrained scalar gain. \n"
      "\n\t"
      "Therefore, the boost along the Stokes V axis \n\t"
      "and the absolute gain will be fixed."
	 << endl;

    for (unsigned ichan=0; ichan<model.size(); ichan++)
      model[ichan]->no_reference_calibrators (false);
  }
  else if (equal_ellipticities)
    for (unsigned ichan=0; ichan<model.size(); ichan++)
      model[ichan]->no_reference_calibrators (true);

  /*
    Time variations are disengaged at the end of the call to solve.
    However, it is desireable to plot the time variation parameters
    after the solution is obtained.  The get_Info method calls this
    function before the model parameters are plotted; therefore, this
    method re-engages the time variations.
  */

  for (unsigned ichan=0; ichan<model.size(); ichan++)
    model[ichan]->engage_time_variations ();

  if (get_prepared())
    return;

  if (previous_cal)
  {
    cerr << "Pulsar::ReceptionCalibrator::initialize using previous solution"
	 << endl;
    for (unsigned ichan=0; ichan<model.size(); ichan++)
      calibrator_estimate[ichan].source
	-> set_stokes( (Stokes< Estimate<double> >)
		       previous_cal->get_stokes (ichan) );
  }
 
  SystemCalibrator::solve_prepare ();

  for (unsigned ichan=0; ichan<fluxcal.size(); ichan++) try
  {
    if (fluxcal[ichan])
      fluxcal[ichan]->update ();
  }
  catch (Error& error)
  {
    model[ichan]->set_valid (false, "Flux calibrator estimate update failed");
  }

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    for (unsigned ichan=0; ichan<pulsar[istate].size(); ichan++)
      pulsar[istate][ichan].update ();

  /*
    The various calls to update_source can incorrectly reset values
    that should remain fixed because they are not free parameters
  */

  setup_calibrators ();
}

void ReceptionCalibrator::check_ready (const char* method, bool unc)
{
  if (get_solved())
    throw Error (InvalidState, method,
		 "Model has been solved. Cannot add data.");

  if (get_prepared())
    throw Error (InvalidState, method,
		 "Model has been initialized. Cannot add data.");

  if (unc && !has_calibrator())
    throw Error (InvalidState, method,
		 "Initial observation required.");
}

/*! Mask invalid SourceEstimate states */
void ReceptionCalibrator::valid_mask
(const std::vector<Calibration::SourceEstimate>& src)
{
  if (src.size () != model.size())
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::valid_mask",
                 "src.size=%d != model.size=%d",
                 src.size (), model.size());

  for (unsigned ichan=0; ichan < model.size(); ichan++)
    model[ichan]->set_valid( model[ichan]->get_valid() && src[ichan].valid );
}


void ReceptionCalibrator::solve_prepare ()
{
  SystemCalibrator::solve_prepare ();

  for (unsigned ichan=0; ichan < model.size(); ichan++)
    if (output_report && model[ichan]->get_valid())
    {
      string name = "fit_goodness_" + tostring(ichan) + ".txt";
      model[ichan]->get_equation()->add_postfit_report( new Calibration::FitGoodnessReport (name) );
    }
}

Pulsar::Calibrator::Info* 
ReceptionCalibrator::new_info_pulsar (unsigned istate) const
{
  SourceInfo* info = new SourceInfo( pulsar[istate] );

  info->set_title( "Stokes Parameters of Phase Bin " 
		   + tostring(pulsar[istate][0].phase_bin) );

  return info;
}

