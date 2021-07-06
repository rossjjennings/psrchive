/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableGain.h"

#include "MEAL/Gain.h"
#include "MEAL/Complex2.h"

using namespace std;
using namespace Calibration;

void VariableGain::init ()
{
  gain = new MEAL::Gain<MEAL::Complex2>;
  set_model( gain );
}

VariableGain::VariableGain ()
{
  init ();
}

VariableGain::VariableGain (const VariableGain& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const VariableGain& 
VariableGain::operator = (const VariableGain& s)
{
  if (&s != this)
    *gain = *(s.gain);
  return *this;
}

VariableGain::~VariableGain ()
{
  if (verbose)
    cerr << "VariableGain::dtor" << endl;
}

//! Return the name of the class
string VariableGain::get_name () const
{
  return "VariableGain";
}

//! Get the instrumental gain, \f$ G \f$, in calibrator flux units
Estimate<double> VariableGain::get_gain () const
{
  if (gain_variation)
    return gain_variation->estimate();
  else
    return gain->get_gain ();
}

//! Set the instrumental gain, \f$ G \f$, in calibrator flux units
void VariableGain::set_gain (const Estimate<double>& g)
{
  if (gain_variation)
    throw Error (InvalidState, "VariableGain::set_gain",
		 "cannot set gain when it is constrained by a function");

  gain->set_gain (g);
}

//! Set the instrumental gain variation
void VariableGain::set_gain_variation (MEAL::Scalar* function)
{
  gain_variation = function;
  set_constraint (0, function);
}

//! Set the instrumental gain variation
const MEAL::Scalar* VariableGain::get_gain_variation () const
{
  return gain_variation.ptr();
}

