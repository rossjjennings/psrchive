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

using namespace std;

static bool verbose = false;

//! Set the response that contains the backend
void Calibration::BackendEstimate::set_response (MEAL::Complex2* xform)
{
  backend = 0;
  
  Calibration::SingleAxis* single_axis;
  single_axis = MEAL::extract<Calibration::SingleAxis>( xform );

  if (single_axis)
  {
    if (verbose)
      cerr << "BackendEstimate::set_response this=" << this <<
	" SingleAxis=" << (void*) single_axis << endl;

    backend = single_axis;

#if _DEBUG
    if (mean)
      {
	cerr << "before condition" << endl;
	mean->update (single_axis);
	MEAL::print (cerr, single_axis);
      }
#endif
    
    // don't delete any mean accumulated to date
    if (!mean || !dynamic_cast<MeanSingleAxis*>( mean.ptr() ))
      mean = new MeanSingleAxis;

    return;
  }
  
  MEAL::Polar* polar_solution;
  polar_solution = MEAL::extract<MEAL::Polar>( xform );

  if (polar_solution)
  {
    if (verbose)
      cerr << "BackendEstimate::set_response Polar" << endl;

    backend = polar_solution;

    // don't delete any mean accumulated to date
    if (!mean || !dynamic_cast<MeanPolar*>( mean.ptr() ))
      mean = new MeanPolar;
    
    return;
  }

  throw Error (InvalidParam, "BackendEstimate::set_response",
	       "unrecognized xform=" + xform->get_name());
}

void Calibration::BackendEstimate::integrate (const MEAL::Complex2* xform)
{
  if (verbose)
    cerr << "BackendEstimate::integrate this=" << this
	 << " mean=" << (void*) mean << " xform=" << xform << endl;

  try {

    if (verbose)
      MEAL::Function::very_verbose = true;

    mean->integrate (xform);

    if (verbose)
    {
      MEAL::Function::very_verbose = false;
      cerr << "BackendEstimate::integrate ok. this=" << this << endl;
    }
      
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
  if (verbose)
  {
    cerr << "BackendEstimate::update this=" << this
	 << " backend=" << (void*) backend << endl;

    MEAL::Function::very_verbose = true;
  }
  
  mean->update (backend);

  if (verbose)
    MEAL::Function::very_verbose = false;
}

bool Calibration::BackendEstimate::spans (const MJD& epoch)
{
  if (verbose)
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

