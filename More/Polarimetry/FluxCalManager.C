/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
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

Calibration::FluxCalManager::FluxCalManager (SignalPath* path)
{
  multiple_source_states = false;
  subtract_off_from_on = false;

  MEAL::Complex2* response = path->get_transformation();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response );

  //
  // It must be possible to extract the frontend, so that the temporal
  // variations applied to pulsar and reference source are not applied
  // to the flux calibrator
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

bool Calibration::FluxCalManager::is_constrained () const
{
  return observations.size();
}

void Calibration::FluxCalManager::add_observation (Signal::Source source_type)
{
  Reference::To<FluxCalObservation> obs = new FluxCalObservation;

  add_backend (obs);
  add_source (obs);

  Reference::To<FluxCalPair> pair = new FluxCalPair;

  // TODO: make pairs and add difference to model

  if (source_type == Signal::FluxCalOn)
    pair->on = obs;
  else if (source_type == Signal::FluxCalOff)
    pair->off = obs;

  observations.push_back( pair );
}


void Calibration::FluxCalManager::add_backend (FluxCalObservation* obs)
{
  obs->backend = new BackendEstimate;

  Reference::To< MEAL::ProductRule<MEAL::Complex2> > fcal_path;
  fcal_path = new MEAL::ProductRule<MEAL::Complex2>;

  if (backend)
  {
    obs->backend->backend = backend->clone();
    fcal_path->add_model( obs->backend->backend );
  }

  fcal_path->add_model ( frontend );

  composite->add_transformation ( fcal_path );

  obs->backend->path_index 
    = composite->get_equation()->get_transformation_index ();
}

void Calibration::FluxCalManager::add_source (FluxCalObservation* obs)
{
  Reference::To< SourceEstimate > source;

  if (multiple_source_states || observations.size() == 0)
  {
    source = new SourceEstimate;

    source->create_source( composite->get_equation() );

    // set the initial guess
    Stokes<double> flux_cal_state (1,0,0,0);
  
    source->source->set_stokes ( flux_cal_state );

    // parameter index of Stokes V
    const unsigned int StokesV = 3;

    // TODO: on or off? ... does the user want to assume PolnCal[V] = 0?
    source->source->set_infit ( StokesV, false );
    source->source->set_Estimate ( StokesV, 0.0);

    string name_prefix = "flux_" + tostring(observations.size()) + "_";
    source->source->set_param_name_prefix( name_prefix );
  }
  else
    // TODO: on or off?
    source = observations[0]->on->source;

  obs->source = source;
}

//! Integrate an estimate of the backend
void FluxCalManager::integrate (Signal::Source type,
				const MEAL::Complex2* xform)
{
  if (!observations.size())
    throw Error (InvalidState, 
		 "Calibration::FluxCalManager::integrate",
		 "no flux calibration backend added to signal path");

  const Calibration::SingleAxis* single
    = dynamic_cast<const Calibration::SingleAxis*> (xform);

  if (single)
    observations.back()->on->backend->estimate.integrate (single);
}

void FluxCalManager::integrate (const Jones< Estimate<double> >& correct,
				const SourceObservation& data)
{
  if (!observations.size())
    throw Error (InvalidState, 
		 "Calibration::FluxCalManager::integrate",
		 "no flux calibration source added to model");

  Stokes< Estimate<double> > result = transform( data.baseline, correct );
  observations.back()->on->source->estimate.integrate (result);
}


void FluxCalManager::submit (CoherencyMeasurementSet& measurements,
			     const Stokes< Estimate<double> >& data)
{
  if (observations.size() == 0)
    throw Error (InvalidState, "Calibration::FluxCalManager::submit",
		 "no flux calibration source/backend added to model");

  FluxCalPair* current = observations.back();

  // TODO: on or off?
  FluxCalObservation* obs = current->on;

  obs->source->add_data_attempts ++;

  try
  {
    Calibration::CoherencyMeasurement state (obs->source->input_index);

    state.set_stokes( data );
    
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

void Calibration::FluxCalManager::update ()
{
  for (unsigned i=0; i < observations.size(); i++)
    observations[i]->update ();
}

void Calibration::FluxCalPair::update ()
{
  if (on)
    on->update();
  if (off)
    off->update();
}

void Calibration::FluxCalObservation::update ()
{
  backend->update();
  source->update();
}
