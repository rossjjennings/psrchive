/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendEstimate.h"
#include "Pulsar/MeanSingleAxis.h"
#include "Pulsar/MeanPolar.h"
#include "Pulsar/SingleAxis.h"

#include "MEAL/Polar.h"
#include "MEAL/ChainRule.h"

using namespace std;

static bool verbose = false;

//! Set the response that contains the backend
void Calibration::BackendEstimate::set_response (MEAL::Complex2* xform)
{
  backend = 0;
  mean = 0;
  
  Calibration::SingleAxis* single_axis;
  single_axis = dynamic_cast<Calibration::SingleAxis*>( xform );

  if (single_axis)
  {
    if (verbose)
      cerr << "BackendEstimate::set_response SingleAxis" << endl;

    backend = single_axis;
    mean = new MeanSingleAxis;
    return;
  }
  
  MEAL::Polar* polar_solution;
  polar_solution = dynamic_cast<MEAL::Polar*>( xform );

  if (polar_solution)
  {
    if (verbose)
      cerr << "BackendEstimate::set_response Polar" << endl;

    backend = polar_solution;
    mean = new MeanPolar;

    return;
  }

  // search for recognized backend component

  MEAL::ChainRule<MEAL::Complex2>* chain;
  chain = dynamic_cast<MEAL::ChainRule<MEAL::Complex2>*>( xform );
  if (chain)
  {
    if (verbose)
      cerr << "BackendEstimate::set_response ChainRule" << endl;

    set_response( chain->get_model () );
    if (backend)
      return;
  }
  
  MEAL::ProductRule<MEAL::Complex2>* product;
  product = dynamic_cast<MEAL::ProductRule<MEAL::Complex2>*>( xform );
  if (product)
  {
    if (verbose)
      cerr << "BackendEstimate::set_response ProductRule" << endl;
    
    for (unsigned imodel=0; imodel<product->get_nmodel(); imodel++)
    {
      set_response( product->get_model(imodel) );
      if (backend)
	return;
    }
  }

  if (!backend)
    throw Error (InvalidParam, "BackendEstimate::set_response",
		 "unrecognized xform=" + xform->get_name());
}

void Calibration::BackendEstimate::integrate (const MEAL::Complex2* xform)
{
  try {

    mean->integrate (xform);
    return;
    
  }
  catch (Error& error)
    {
      if (verbose)
	cerr << "BackendEstimate::integrate Mean::integrate failed: "
	     << error.get_message() << endl;
    }

  // search for recognized backend component
  
  const MEAL::ProductRule<MEAL::Complex2>* product;
  product = dynamic_cast<const MEAL::ProductRule<MEAL::Complex2>*>( xform );
  if (product)
  {
    if (verbose)
      cerr << "BackendEstimate::integrate ProductRule" << endl;
    
    for (unsigned imodel=0; imodel<product->get_nmodel(); imodel++)
    {
      try {

	mean->integrate ( product->get_model(imodel) );
	return;
    
      }
      catch (Error& error)
      {
	if (verbose)
	  cerr << "BackendEstimate::integrate Mean::integrate"
	    " ProductRule[" << imodel << " failed: "
	       << error.get_message() << endl;
      }
    }
  }

  throw Error (InvalidParam, "BackendEstimate::integrate"
	       "unrecognized transformation");

}

void Calibration::BackendEstimate::update ()
{
  mean->update (backend);
}

bool Calibration::BackendEstimate::spans (const MJD& epoch)
{
  cerr << "BackendEstimate::spans epoch=" << epoch
       << " start=" << start_time
       << " end=" << end_time << endl;
  
  return epoch > start_time && epoch < end_time;
}

//! update min_time and max_time
void Calibration::BackendEstimate::add_observation_epoch (const MJD& epoch)
{
  MJD zero;

  if (min_time == zero || epoch < min_time) 
    min_time = epoch;

  if (max_time == zero || epoch > max_time) 
    max_time = epoch;
}

