/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableBackend.h"
#include "Pulsar/SingleAxis.h"

#include "Pauli.h"
#include "Error.h"

using namespace std;
using namespace Calibration;

void VariableBackend::init ()
{
  backend = new SingleAxis;
  set_model( backend );
}

VariableBackend::VariableBackend ()
{
  init ();
}

VariableBackend::VariableBackend (const VariableBackend& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const VariableBackend& 
VariableBackend::operator = (const VariableBackend& s)
{
  if (&s != this)
    *backend = *(s.backend);
  return *this;
}

VariableBackend::~VariableBackend ()
{
  if (verbose)
    cerr << "VariableBackend::dtor" << endl;
}

//! Return the name of the class
string VariableBackend::get_name () const
{
  return "VariableBackend";
}

//! Set cyclical bounds on the differential phase
void VariableBackend::set_cyclic (bool flag)
{
   backend->set_cyclic (flag);
}


//! Get the instrumental gain, \f$ G \f$, in calibrator flux units
Estimate<double> VariableBackend::get_gain () const
{
  if (gain_variation)
    return gain_variation->estimate();
  else
    return backend->get_gain ();
}

//! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
Estimate<double> VariableBackend::get_diff_gain () const
{
  if (diff_gain_variation)
    return diff_gain_variation->estimate();
  else
    return backend->get_diff_gain ();
}

//! Get the differential phase, \f$ \phi \f$, in radians
Estimate<double> VariableBackend::get_diff_phase () const
{
  if (diff_phase_variation)
    return diff_phase_variation->estimate();
  else
    return backend->get_diff_phase ();
}

//! Set the instrumental gain, \f$ G \f$, in calibrator flux units
void VariableBackend::set_gain (const Estimate<double>& g)
{
  if (gain_variation)
    throw Error (InvalidState, "VariableBackend::set_gain",
		 "cannot set gain when it is constrained by a function");

  backend->set_gain (g);
}

//! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
void VariableBackend::set_diff_gain (const Estimate<double>& gamma)
{
  if (diff_gain_variation)
    throw Error (InvalidState, "VariableBackend::set_diff_gain",
		 "cannot set diff_gain when it is constrained by a function");

  backend->set_diff_gain (gamma);
}
   
//! Set the differential phase, \f$ \phi \f$, in radians
void VariableBackend::set_diff_phase (const Estimate<double>& phi)
{
  if (diff_phase_variation)
    throw Error (InvalidState, "VariableBackend::set_diff_phase",
		 "cannot set diff_phase when it is constrained by a function");

  backend->set_diff_phase (phi);
}

SingleAxis* VariableBackend::get_backend ()
{
  return backend;
}

const SingleAxis* VariableBackend::get_backend () const
{
  return backend;
}

//! Set the instrumental gain variation
void VariableBackend::set_gain_variation (MEAL::Scalar* function)
{
  gain_variation = function;
  set_constraint (0, function);
}

//! Set the differential gain variation
void VariableBackend::set_diff_gain_variation (MEAL::Scalar* function)
{
  diff_gain_variation = function;
  set_constraint (1, function);
}

//! Set the differential phase variation
void VariableBackend::set_diff_phase_variation (MEAL::Scalar* function)
{
  diff_phase_variation = function;
  set_constraint (2, function);
}

//! Set the instrumental gain variation
const MEAL::Scalar* VariableBackend::get_gain_variation () const
{
  return gain_variation.ptr();
}

//! Set the differential gain variation
const MEAL::Scalar* VariableBackend::get_diff_gain_variation () const
{
  return diff_gain_variation.ptr();
}
   
//! Set the differential phase variation
const MEAL::Scalar* VariableBackend::get_diff_phase_variation () const
{
  return diff_phase_variation.ptr();
}
 
