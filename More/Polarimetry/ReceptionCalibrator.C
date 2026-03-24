/***************************************************************************
 *
 *   Copyright (C) 2003-2025 by Willem van Straten
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

#include "debug.h"

#include <algorithm>
#include <assert.h>

using namespace std;
using Pulsar::ReceptionCalibrator;
using Calibration::FluxCalManager;

ReceptionCalibrator::ReceptionCalibrator (Calibrator::Type* _type)
{
  type = _type;

  degenerate_V_boost = true;
  degenerate_V_rotation = true;

  measure_cal_V = true;
  measure_cal_Q = false;
  equal_ellipticities = false;

  normalize_by_invariant = false;
  independent_gains = false;

  multiple_flux_calibrators = false;
  model_fluxcal_on_minus_off = false;
  
  check_pointing = false;
  physical_coherency = false;

  output_report = false;

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
  standard_data->set_normalize (normalize_by_invariant);
  standard_data->select_profile( p );

  if (verbose)
  {
    ProfileStats* stats = standard_data->get_poln_stats()->get_stats();

    cerr << "Pulsar::ReceptionCalibrator::set_standard_data standard_data=" << standard_data.ptr()
         << " stats=" << stats << endl
         << "Pulsar::ReceptionCalibrator::set_standard_data normalize_by_invariant=" << normalize_by_invariant << endl;
  }

  ensure_consistent_onpulse ();
}

void ReceptionCalibrator::ensure_consistent_onpulse ()
{
  if (!standard_data)
    return;

  ProfileStats* stats = standard_data->get_poln_stats()->get_stats();

  if (verbose)
  {
    cerr << "Pulsar::ReceptionCalibrator::ensure_consistent_onpulse standard_data=" << standard_data.ptr()
         << " stats=" << stats << endl
         << "Pulsar::ReceptionCalibrator::ensure_consistent_onpulse phase bins=";
  }

  for (unsigned istate=0; istate < phase_bins.size(); istate++)
  {
    if (verbose)
    {
      if (istate > 0)
        cerr << ",";
      cerr << phase_bins[istate];
    }
    stats->set_onpulse (phase_bins[istate], true);
  }

  if (verbose)
    cerr << endl;
}

const Pulsar::PhaseWeight* ReceptionCalibrator::get_baseline () const
{
  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::get_baseline standard_data=" << standard_data.ptr()
         << " stats=" << standard_data->get_poln_stats()->get_stats() << endl;
  return standard_data->get_poln_stats()->get_stats()->get_baseline();
}

//! Get the on-pulse mask
const Pulsar::PhaseWeight* ReceptionCalibrator::get_onpulse () const
{
  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::get_onpulse standard_data=" << standard_data.ptr()
         << " stats=" << standard_data->get_poln_stats()->get_stats() << endl;
  return standard_data->get_poln_stats()->get_stats()->get_onpulse();
}

void ReceptionCalibrator::set_normalize_by_invariant (bool set)
{
  normalize_by_invariant = set;

  if (!standard_data)
    return;

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::set_normalize_by_invariant standard_data=" << standard_data.ptr()
         << " stats=" << standard_data->get_poln_stats()->get_stats() << endl;
         
  standard_data->set_normalize (normalize_by_invariant);
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
  {
    if (verbose)
      cerr << "Pulsar::ReceptionCalibrator::initial_observation create new standard_data" << endl;
    set_standard_data (data);
  }

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
    load_previous();
  }

  assert( pulsar_estimate.size() == phase_bins.size() );

  cerr << "ReceptionCalibrator::initial_observation"
    " initialize pulsar source estimates" << endl;

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar_estimate.size(); istate++)
    init_estimates ( pulsar_estimate[istate], phase_bins[istate] );

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

  if (measure_cal_Q && degenerate_V_rotation)
    model[ichan] -> fix_orientation ();
}

void ReceptionCalibrator::submit_calibrator_data ()
{
  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::submit_calibrator_data" << endl;

  SystemCalibrator::submit_calibrator_data ();

  if (!fluxcal.size())
    return;

  const unsigned nchan = get_nchan();
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!fluxcal.at(ichan))
    {
      cerr << "no fluxcal ichan=" << ichan << endl;
      continue;
    }
    
    if (fluxcal.at(ichan)->is_constrained())
      continue;

    if (!model[ichan]->get_valid())
      continue;

    std::string why = fluxcal.at(ichan)->why_not_constrained();

    if (verbose > 1)
      cerr << "ichan=" << ichan << " flux calibrator not constrained: "
	   << why << endl;

    model[ichan]->set_valid (false, why.c_str());
  }
}


//! Add the specified pulse phase bin to the set of state constraints
void ReceptionCalibrator::add_state (int phase_bin)
{
  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << phase_bin << endl;

  for (unsigned istate=0; istate<phase_bins.size(); istate++)
  {
    if (phase_bins[istate] == phase_bin)
    {
      cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << phase_bin << " already in use" << endl;
      return;
    }
  }

  phase_bins.push_back (phase_bin);

  pulsar_estimate.resize( pulsar_estimate.size() + 1 );

  if (has_calibrator())
    init_estimates( pulsar_estimate.back(), phase_bin );
}


//! Get the number of pulsar phase bin input polarization states
unsigned ReceptionCalibrator::get_nstate_pulsar () const
{
  return pulsar_estimate.size();
}

bool equal_pi (const Angle& a, const Angle& b, float tolerance = 0.01);

bool ReceptionCalibrator::match (const Archive* data, bool throw_exception)
{
  if (verbose > 1)
    cerr << "ReceptionCalibrator::match name=" << get_name() << " data.source=" << data->get_source() << endl;
  
  if (get_name() != data->get_source())
  {
    if (verbose > 1)
      cerr << "ReceptionCalibrator::match return false" << endl;
    return false;
  }

  if (!has_calibrator())
  {
    if (verbose)
    cerr << "ReceptionCalibrator::match initial observation for data.source=" << data->get_source() << endl;

    initial_observation (data);
  }

  return SystemCalibrator::match (data, throw_exception);
}

double ReceptionCalibrator::get_invariant (Integration* subint, unsigned ichan)
{
  if (!standard_data)
    throw Error (InvalidState, "ReceptionCalibrator::get_invariant", "standard_data not set");

  standard_data->set_profile( subint->new_PolnProfile (ichan) );
  Estimate<double> total_squared_invariant = standard_data->get_total_squared_invariant();
  return sqrt(total_squared_invariant.get_value());
}

void ReceptionCalibrator::add_pulsar
( Calibration::CoherencyMeasurementSet& measurements,
  const Integration* integration, unsigned ichan )
{
  DEBUG("Pulsar::ReceptionCalibrator::add_pulsar standard_data=" << standard_data.ptr()
        << " stats=" << standard_data->get_poln_stats()->get_stats());
         
  standard_data->set_profile( integration->new_PolnProfile (ichan) );

  if (report_total_invariant && normalize_by_invariant)
  {
    Estimate<double> invint = standard_data->get_total_squared_invariant();
    invint_out << integration->get_epoch().printdays(10) << " " << ichan << " " << sqrt(invint.get_value()) << endl;
    if (!invint_out)
    {
      cerr << "ReceptionCalibrator::add_pulsar FAILED to write total invariant for ichan=" << ichan << endl;
    }
  }

  for (unsigned istate=0; istate < pulsar_estimate.size(); istate++)
  {
    if (ichan >= pulsar_estimate[istate].size())
      throw Error (InvalidParam, "ReceptionCalibrator::add_pulsar",
                  "ichan=%u >= pulsar[%u].size=%u", ichan, istate, pulsar_estimate[istate].size());

    add_data (measurements, pulsar_estimate[istate][ichan], integration->get_epoch(), ichan);
  }
}


void ReceptionCalibrator::add_data
( vector<Calibration::CoherencyMeasurement>& bins,
  Calibration::SourceEstimate* estimate,
  const MJD& epoch,
  unsigned ichan )
{
  get_data_call ++;

  int ibin = estimate->phase_bin;

  DEBUG("Pulsar::ReceptionCalibrator::add_data standard_data=" << standard_data.ptr()
        << " stats=" << standard_data->get_poln_stats()->get_stats());

  try
  {
    Stokes< Estimate<double> > stokes;
    
    if (ibin == Calibration::SourceEstimate::baseline_mean)
      stokes = standard_data->get_baseline();
    else
      stokes = standard_data->get_stokes( ibin );

    // NOTE: the measured states are not corrected / calibrated
    Calibration::CoherencyMeasurement state (estimate->input_index);
    state.set_stokes( stokes );
    bins.push_back ( state );
  }
  catch (Error& error)
  {
    if (verbose > 1)
      cerr << "Pulsar::ReceptionCalibrator::add_data ichan=" << ichan 
	   << " ibin=" << ibin << " error\n\t" << error.get_message() << endl;

    get_data_fail ++;
  }
}

Calibration::SourceEstimate* ReceptionCalibrator::get_estimate (unsigned index, unsigned ichan)
{
  for (unsigned istate=0; istate < pulsar_estimate.size(); istate++)
  {
    Calibration::SourceEstimate* estimate = pulsar_estimate.at(istate).at(ichan);
    if (estimate->input_index == index)
      return estimate;
  }

  throw Error (InvalidParam, "ReceptionCalibrator::get_estimate",
	       "no match for index=%u ichan=%u", index, ichan);
}
      
void ReceptionCalibrator::integrate_pulsar_data
(const Calibration::CoherencyMeasurementSet& data)
{
  unsigned ichan = data.get_ichan();
  MJD epoch = data.get_epoch();

  data.set_coordinates();

  for (unsigned i=0; i < data.size(); i++)
  {
    unsigned index = data[i].get_input_index();
    Calibration::SourceEstimate* estimate = get_estimate (index, ichan);

    integrate_pulsar_data (data[i], estimate, epoch, ichan);
  }
}

void ReceptionCalibrator::integrate_pulsar_data
(const Calibration::CoherencyMeasurement& data,
 Calibration::SourceEstimate* estimate,
 const MJD& epoch,
 unsigned ichan) try
{
  estimate->add_data_attempts ++;

  if (Profile::verbose)
    cerr << "ReceptionCalibrator::integrate_pulsar_data chan=" << ichan
         << " calling Calibration::CoherencyMeasurement::set_coordinates" << endl;

  data.set_coordinates();

  auto transformation = get_transformation (epoch, Signal::Pulsar, ichan);

  /* Correct the stokes parameters using the current best estimate of
     the instrument and the parallactic angle rotation before adding
     them to best estimate of the input state */

  Jones< Estimate<double> > xform = transformation->evaluate();

  if (Profile::verbose)
    cerr << "ReceptionCalibrator::integrate_pulsar_data chan=" << ichan << " xform=" << xform << endl;

  auto correct = inv(xform);
  Stokes< Estimate<double> > stokes = data.get_stokes();
  stokes = transform( stokes, correct );
    
  estimate->estimate.integrate( stokes );
}
catch (Error& error)
{
  unsigned ibin = estimate->phase_bin;

  // if (verbose > 1)
  cerr << "Pulsar::ReceptionCalibrator::add_data ichan=" << ichan 
       << " ibin=" << ibin << " error\n\t" << error.get_message() << endl;
  estimate->add_data_failures ++;  
}

void ReceptionCalibrator::prepare_calibrator_estimate (Signal::Source source)
{
  SystemCalibrator::prepare_calibrator_estimate (source);

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::prepare_calibrator_estimate" << endl;

  if (source != Signal::FluxCalOn && source != Signal::FluxCalOff)
    return;

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::prepare_calibrator_estimate "
	 << source << " nchan=" << get_nchan() << endl;

  const unsigned nchan = get_nchan();

  if (model.size() != nchan)
    throw Error (InvalidState,
		 "ReceptionCalibrator::prepare_calibrator_estimate",
		 "model.size()=%u != nchan=%u", model.size(), nchan);
  
  if (fluxcal.size() == 0)
  {
    fluxcal.resize( nchan );

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ichan])
      {
        cerr << "no model ichan=" << ichan;
        continue;
      }
      
      if (!model[ichan]->get_valid())
        continue;

      fluxcal[ichan] = new Calibration::FluxCalManager( model[ichan] );
      fluxcal[ichan]->model_multiple_source_states( multiple_flux_calibrators );
      fluxcal[ichan]->model_on_minus_off( model_fluxcal_on_minus_off );
    }
  }

  fluxcal_observation_added.resize( nchan );
  for (unsigned ichan=0; ichan<nchan; ichan++)
    fluxcal_observation_added[ichan] = false;
}

void ReceptionCalibrator::setup_calibrators ()
{
  if (verbose > 1)
    cerr << "ReceptionCalibrator::setup_calibrators" << endl;
  
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

void set_fixed_QUV ( Calibration::SourceEstimate* cal, double value )
{
  for (unsigned ipol=1; ipol < 4; ipol++)
    if (!cal->source->get_infit (ipol))
      cal->source->set_Estimate (ipol, value);
}

void ReceptionCalibrator::setup_poln_calibrator (Calibration::SourceEstimate* est)
{
  Signal::Basis basis = get_calibrator()->get_basis ();

  for (unsigned istokes=0; istokes<4; istokes++)
    est->source->set_infit (istokes, false);

  // calibrator flux is unity by definition
  est->source->set_Estimate (0, 1.0);
    
  if (basis == Signal::Linear)
  {
    // degree of polarization (Stokes U) may vary
    est->source->set_infit (2, true);
    
    if (measure_cal_Q)
      est->source->set_infit (1, true);
  }
  else
  {
    // degree of polarization (Stokes Q) may vary
    est->source->set_infit (1, true);
  }
  
  bool cal_V_may_vary = equal_ellipticities || !degenerate_V_boost || has_fluxcal();

  if (measure_cal_V && cal_V_may_vary)
  {
    if (verbose)
      cerr << "Stokes V of the calibrator may vary" << endl;
    est->source->set_infit (3, true);
  }

  set_fixed_QUV (est, 0.0);
}

void ReceptionCalibrator::setup_flux_calibrator (FluxCalManager* mgr)
{
  Signal::Basis basis = get_calibrator()->get_basis ();
  
  if (basis == Signal::Circular || measure_cal_V)
  {
    if (verbose)
      cerr << "ReceptionCalibrator::setup_flux_calibrator ok" << endl;
  }
  else
    mgr->allow_StokesV_to_vary ();
}

bool ReceptionCalibrator::has_fluxcal () const
{
  return fluxcal.size() != 0;
}

const Calibration::FluxCalManager*
ReceptionCalibrator::get_fluxcal (unsigned ichan) const
{
  if (!fluxcal.at(ichan))
    throw Error (InvalidState, "ReceptionCalibrator::get_fluxcal",
		 "fluxcal[%u] is null", ichan);
  
  return fluxcal.at(ichan);
}

bool is_FluxCal_observation (Signal::Source source)
{
  return source == Signal::FluxCalOn || source == Signal::FluxCalOff;
}

void ReceptionCalibrator::submit_calibrator_data 
(
 Calibration::CoherencyMeasurementSet& measurements,
 const Calibration::SourceObservation& data
 )
{
  if (!is_FluxCal_observation(data.source))
  {
    SystemCalibrator::submit_calibrator_data (measurements, data);
    return;
  }

  if (fluxcal[data.ichan])
  {
    if (verbose > 2)
      cerr << "ReceptionCalibrator::submit_calibrator_data fluxcal ichan=" << data.ichan << endl;
    
    if (!fluxcal_observation_added[data.ichan])
      fluxcal[data.ichan]->add_observation (data.source);

    fluxcal_observation_added[data.ichan] = true;

    fluxcal[data.ichan]->submit (measurements, data);
  }
}

void ReceptionCalibrator::integrate_calibrator_data
(const Calibration::SourceObservation& data)
{
  if (data.source == Signal::FluxCalOn || data.source == Signal::FluxCalOff)
  {
    if (verbose > 2)
      cerr << "ReceptionCalibrator::integrate_calibrator_data fluxcal ichan=" << data.ichan << endl;

    Jones< Estimate<double> > apply = data.response;

    if (previous)
     apply = previous->get_response (data.ichan);

    apply = invert_basis * apply;

    if (fluxcal[data.ichan])
      fluxcal[data.ichan]->integrate (apply, data);
  }
  else
    SystemCalibrator::integrate_calibrator_data (data);
}

void ReceptionCalibrator::integrate_calibrator_solution
(const Calibration::SourceObservation& data)
{
  if (data.source == Signal::FluxCalOn || data.source == Signal::FluxCalOff)
  {
    if (verbose > 2)
      cerr << "ReceptionCalibrator::integrate_calibrator_solution fluxcal ichan=" << data.ichan << endl;

    if (fluxcal[data.ichan])
      fluxcal[data.ichan]->integrate (data.source, data.xform);
  }
  else
    SystemCalibrator::integrate_calibrator_solution (data);
}

void ReceptionCalibrator::export_prepare () const
{
  const_cast<ReceptionCalibrator*>(this)->solve_prepare();
}

void ReceptionCalibrator::solve_prepare () try
{
  bool set_equal_ellipticities = equal_ellipticities;
  bool fit_gain = true;

  if (calibrator_estimate.size() == 0)
  {
    cerr <<
      "Pulsar::ReceptionCalibrator::solve_prepare WARNING: \n\t"
      "Without a ReferenceCalibrator observation, there remains \n\t";
    if (degenerate_V_boost)
      cerr << "a degeneracy along the Stokes V axis and \n\t";
    cerr << "an unconstrained scalar gain.  Therefore, \n\t";

    if (degenerate_V_boost) 
    {
      cerr << "the boost along the Stokes V axis and \n\t";
      set_equal_ellipticities = true;
    }
    cerr << "the absolute gain will be fixed." << endl;
    fit_gain = false;
  }

  if (set_equal_ellipticities)
    for (unsigned ichan=0; ichan<model.size(); ichan++)
      model[ichan]->equal_ellipticities();

  if (!fit_gain)
    for (unsigned ichan=0; ichan<model.size(); ichan++)
      model[ichan]->fit_gain(false);

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
    cerr << "Pulsar::ReceptionCalibrator::initialize using previous solution" << endl;
    for (unsigned ichan=0; ichan<model.size(); ichan++)
    {
      Stokes<Estimate<double>> calS = previous_cal->get_stokes (ichan);
      calibrator_estimate[ichan]->source->set_stokes( calS );
    }
  }
 
  SystemCalibrator::solve_prepare ();

  for (unsigned ichan=0; ichan < model.size(); ichan++)
    if (output_report && model[ichan]->get_valid())
    {
      string name = "fit_goodness_" + tostring(ichan) + ".txt";
      model[ichan]->get_equation()->add_postfit_report( new Calibration::FitGoodnessReport (name) );
    }
    
  for (unsigned ichan=0; ichan<fluxcal.size(); ichan++) try
  {
    if (fluxcal[ichan])
      fluxcal[ichan]->update ();
  }
  catch (Error& error)
  {
    model[ichan]->set_valid (false, "Flux calibrator estimate update failed");
  }

  for (unsigned istate=0; istate<pulsar_estimate.size(); istate++)
  {
    if (report_input_failed)
      SystemCalibrator::print_input_failed (pulsar_estimate[istate]);

    for (unsigned ichan=0; ichan<pulsar_estimate[istate].size(); ichan++)
      if (pulsar_estimate[istate][ichan]) 
        pulsar_estimate[istate][ichan]->update ();
  }

  if (report_input_failed)
    SystemCalibrator::close_input_failed ();

  /*
    The various calls to update_source can incorrectly reset values
    that should remain fixed because they are not free parameters
  */

  setup_calibrators ();

  /*
    Any configuration set on the command line take precedence
  */
  for (unsigned ichan=0; ichan<model.size(); ichan++)
  {
    configure (model[ichan]->get_equation());
  }
}
catch (Error& error)
{
  throw error += "ReceptionCalibrator::solve_prepare";
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

Pulsar::Calibrator::Info* 
ReceptionCalibrator::new_info_pulsar (unsigned istate) const
{
  SourceInfo* info = new SourceInfo( pulsar_estimate[istate] );

  for (unsigned i=0; i < pulsar_estimate[istate].size(); i++)
    if (pulsar_estimate[istate][i])
    {
      int bin = pulsar_estimate[istate][i]->phase_bin;
      if (bin == Calibration::SourceEstimate::baseline_mean)
        info->set_title( "Stokes Parameters of the Baseline Mean" );
      else
        info->set_title( "Stokes Parameters of Phase Bin " + tostring(bin) );
      break;
    }

  return info;
}

