/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableBackendEstimate.h"
#include "Pulsar/SingleAxis.h"
#include "MEAL/Polar.h"

using namespace std;

static bool verbose = false;

void Calibration::BackendEstimate::update ()
{
  SingleAxis* single = dynamic_cast<SingleAxis*>( backend.get() );
  if (!single)
    return;

  if (gain)
    update( gain, single->get_gain().get_value() );
  
  if (diff_gain)
    update( diff_gain, single->get_diff_gain().get_value() );
    
  if (diff_phase)
    update ( diff_phase, single->get_diff_phase().get_value() );
}

void Calibration::BackendEstimate::update (MEAL::Scalar* function, double value)
{
  MEAL::Polynomial* polynomial = dynamic_cast<MEAL::Polynomial*>( function );
  if (polynomial)
    polynomial->set_param( 0, value );
}

void Calibration::BackendEstimate::set_gain (Univariate<Scalar>* function)
{
  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "Calibration::BackendEstimate::set_gain",
		 "cannot set gain variation in polar model");

  physical->set_gain( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  gain = function;
}

void Calibration::BackendEstimate::set_diff_gain (Univariate<Scalar>* function)
{
  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "Calibration::BackendEstimate::set_diff_gain",
		 "cannot set gain variation in polar model");

  physical -> set_diff_gain( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_gain = function;
}

void Calibration::BackendEstimate::set_diff_phase (Univariate<Scalar>* function)
{
  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "Calibration::BackendEstimate::set_diff_phase",
		 "cannot set diff_phase variation in polar model");

  physical -> set_diff_phase( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_phase = function;
}
