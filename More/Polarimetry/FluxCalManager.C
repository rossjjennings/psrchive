/***************************************************************************
 *
 *   Copyright (C) 2012 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalManager.h"
#include "Pulsar/BackendFeed.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include "Pauli.h"

using namespace std;
using Calibration::FluxCalManager;

// #define _DEBUG 1

Calibration::FluxCalManager::FluxCalManager (SignalPath* path) try
{
  multiple_source_states = false;
  subtract_off_from_on = false;
  StokesV_may_vary = false;

  MEAL::Complex2* response = path->get_transformation();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response );

  //
  // It must be possible to separate the frontend and backend
  // transformations , so that the temporal variations applied to
  // pulsar and reference source are not applied to the flux
  // calibrator
  //

  if (!physical)
    throw Error (InvalidState,
	         "Calibration::FluxCalManager ctor",
	         "Backend/Feed parameterization required to model fluxcal");

  composite = path;

  backend = new Calibration::SingleAxis;

  frontend = new MEAL::ProductRule<MEAL::Complex2>;

  frontend->add_model( physical->get_frontend() );

  if (path->has_basis())
    frontend->add_model( path->get_basis() );
}
catch (Error& error)
{
  throw error += "FluxCalManager (SignalPath*)";
}

bool Calibration::FluxCalManager::is_constrained () const
{
  if (on_observations.size() == 0)
  {
    not_constrained_reason = "no FluxCal-On observations";
    return false;
  }

  if (subtract_off_from_on && off_observations.size() == 0)
  {
    not_constrained_reason = "no FluxCal-Off observations";
    return false;
  }

  return true;
}

void Calibration::FluxCalManager::add_observation (Signal::Source source_type)
{
  Reference::To<FluxCalObservation> obs = new FluxCalObservation;

  obs->source_type = source_type;
  
  add_backend (obs);
  add_source (obs);

  if (source_type == Signal::FluxCalOn)
    on_observations.push_back( obs );

  else if (source_type == Signal::FluxCalOff)
    off_observations.push_back( obs );  
}


void Calibration::FluxCalManager::add_backend (FluxCalObservation* obs)
{
  obs->backend = new BackendEstimate;

  Reference::To< MEAL::ProductRule<MEAL::Complex2> > fcal_path;
  fcal_path = new MEAL::ProductRule<MEAL::Complex2>;

  if (backend)
  {
    MEAL::Complex2* clone = backend->clone(); 
    obs->backend->set_response (clone);
    fcal_path->add_model (clone);
  }

  fcal_path->add_model ( frontend );

  composite->add_transformation ( fcal_path );

  obs->backend->path_index 
    = composite->get_equation()->get_transformation_index ();
}

Calibration::FluxCalObsVector&
Calibration::FluxCalManager::get_observations (Signal::Source source_type)
{
  if (source_type == Signal::FluxCalOn)
    return on_observations;
  if (source_type == Signal::FluxCalOff)
    return off_observations;

  throw Error (InvalidParam, "Calibration::FluxCalManager::get_observations",
	       "invalid Signal::Source type=" + tostring(source_type));
}

void Calibration::FluxCalManager::add_source (FluxCalObservation* obs)
{
  FluxCalObsVector& observations = get_observations (obs->source_type);
  
  if (multiple_source_states || observations.size() == 0)
    obs->source = create_SourceEstimate (obs->source_type);
  else
    obs->source = observations.at(0)->source;
}

Calibration::SourceEstimate*
Calibration::FluxCalManager::create_SourceEstimate (Signal::Source source_type)
{
  Reference::To<SourceEstimate> source_estimate;

  if (subtract_off_from_on)
  {
    if (!standard_candle)
      standard_candle = new SourceDeltaEstimate;

    if (source_type == Signal::FluxCalOn)
      source_estimate = standard_candle;
    else
      source_estimate = new SourceEstimate;
  }
  else
    source_estimate = new SourceEstimate;

  source_estimate->create_source( composite->get_equation() );

  if (subtract_off_from_on && source_type == Signal::FluxCalOff)
  {
    if (!standard_candle->has_total())
      throw Error (InvalidState, "FluxCalManager::create_SourceEstimate", 
                   "FluxCal-Off with no matching FluxCal-On");

    standard_candle->set_baseline (source_estimate);
  }

  // set the initial guess
  Stokes<double> flux_cal_state (1,0,0,0);
  source_estimate->source->set_stokes ( flux_cal_state );

  if (!StokesV_may_vary && source_type == Signal::FluxCalOn)
  {
    // parameter index of Stokes V
    const unsigned int StokesV = 3;

    source_estimate->source->set_infit ( StokesV, false );
    source_estimate->source->set_Estimate ( StokesV, 0.0 );
  }
    
  string name_prefix = "flux_";
  if (source_type == Signal::FluxCalOn)
    name_prefix += "on_";
  else
    name_prefix += "off_";

  if (multiple_source_states)
  {
    FluxCalObsVector& observations = get_observations (source_type);
    name_prefix += tostring(observations.size()) + "_";
  }

  source_estimate->source->set_param_name_prefix( name_prefix );

  return source_estimate.release();
}

void Calibration::FluxCalManager::allow_StokesV_to_vary (bool flag)
{
  StokesV_may_vary = flag;

  set_StokesV_infit (on_observations);
  set_StokesV_infit (off_observations);
}

void Calibration::FluxCalManager::model_multiple_source_states (bool flag)
{
  if (on_observations.size() > 0 || off_observations.size() > 0)
    throw Error (InvalidState, "FluxCalManager::model_multiple_source_states",
		 "observations already added; set this flag before adding");

  multiple_source_states = flag;
}

void Calibration::FluxCalManager::model_on_minus_off (bool flag)
{
  if (on_observations.size() > 0 || off_observations.size() > 0)
    throw Error (InvalidState, "FluxCalManager::model_on_minus_off",
		 "observations already added; set this flag before adding");

  subtract_off_from_on = flag;
}

void FluxCalManager::set_StokesV_infit (FluxCalObsVector& observations)
{
  // parameter index of Stokes V
  const unsigned int StokesV = 3;

  for (unsigned i=0; i<observations.size(); i++)
  {
    Reference::To< SourceEstimate > source = observations.at(i)->source;
    source->source->set_infit ( StokesV, StokesV_may_vary );
  }  
}

//! Integrate an estimate of the backend
void FluxCalManager::integrate (Signal::Source type,
				const MEAL::Complex2* xform)
{
  FluxCalObsVector& observations = get_observations (type);
  
  if (!observations.size())
    throw Error (InvalidState, 
		 "Calibration::FluxCalManager::integrate",
		 "no flux calibration backend added to signal path");

  observations.back()->backend->integrate (xform);
}

void FluxCalManager::integrate (const Jones< Estimate<double> >& correct,
				const SourceObservation& data)
{
  FluxCalObsVector& observations = get_observations (data.source);

  if (!observations.size())
    throw Error (InvalidState, 
		 "Calibration::FluxCalManager::integrate",
		 "no flux calibration source added to model");

  Stokes< Estimate<double> > result = transform( data.baseline, correct );

  observations.back()->source->estimate.integrate (result);
}


void FluxCalManager::submit (CoherencyMeasurementSet& measurements,
			     const SourceObservation& data)
{
  FluxCalObsVector& observations = get_observations (data.source);
   
  if (observations.size() == 0)
    throw Error (InvalidState, "Calibration::FluxCalManager::submit",
		 "no flux calibration source/backend added to model");

  FluxCalObservation* obs = observations.back();

  obs->source->add_data_attempts ++;

  try
  {
    Calibration::CoherencyMeasurement state (obs->source->input_index);

    state.set_stokes( data.baseline );
    
    measurements.push_back (state);
    
    unsigned index = obs->backend->get_path_index();
    measurements.set_transformation_index (index);
    
    DEBUG ("FluxCalManager::submit ichan=" << ichan);
    
    composite->get_equation()->add_data (measurements);
  }
  catch (Error& error)
    {
      obs->source->add_data_failures ++;
      throw error += "Calibration::FluxCalManager::submit";
    }
}

unsigned Calibration::FluxCalManager::get_nstate_on () const
{
  if (multiple_source_states)
    return off_observations.size();
  else
    return 1;
}

const Calibration::SourceEstimate*
FluxCalManager::get_source_on (unsigned istate) const
{
  return on_observations.at(istate)->source;
}


const Calibration::SourceEstimate*
FluxCalManager::get_source_off (unsigned istate) const
{
  return off_observations.at(istate)->source;
}


//! Get the number of independent FluxCalOff source states
unsigned Calibration::FluxCalManager::get_nstate_off () const
{
  if (multiple_source_states)
    return off_observations.size();
  else
    return 1;
}

void Calibration::FluxCalManager::update ()
{
  for (unsigned i=0; i < on_observations.size(); i++)
    on_observations[i]->update ();

  for (unsigned i=0; i < off_observations.size(); i++)
    off_observations[i]->update ();
}

void Calibration::FluxCalObservation::update ()
{
  backend->update();
  source->update();
}
