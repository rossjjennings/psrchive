/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableTransformation.h"

#include "Pauli.h"
#include "Error.h"

using namespace std;
using namespace Calibration;

// #define _DEBUG 1

void VariableTransformation::init()
{
  add_model (&post_correction);
  add_model (&chain);
  add_model (&pre_correction);
}

VariableTransformation::VariableTransformation ()
{
  init ();
}

VariableTransformation::VariableTransformation (const VariableTransformation& s)
{
  init ();
  operator = (s);
}

//! Assignment Operator
const VariableTransformation& 
VariableTransformation::operator = (const VariableTransformation& s)
{
  if (&s == this)
    return *this;

  // clone the model
  if (s.model)
    set_model (s.model->clone());

  // clone any constraints
  for (auto f: s.function)
    set_constraint (f.first, f.second->clone());

  return *this;
}

VariableTransformation::~VariableTransformation ()
{
#if _DEBUG
  cerr << "VariableTransformation::dtor this=" << this << endl;
#endif
}

VariableTransformation* VariableTransformation::clone () const
{
  return new VariableTransformation (*this);
}

void VariableTransformation::set_model (MEAL::Complex2* _model)
{
  model = _model;
  chain.set_model (model);
}

MEAL::Complex2* VariableTransformation::get_model ()
{
  return model;
}

const MEAL::Complex2* VariableTransformation::get_model () const
{
  return model;
}

//! Set the multivariate function that constrains the specified parameter
void VariableTransformation::set_constraint (unsigned index, MEAL::Nvariate<MEAL::Scalar>* func)
{
#if _DEBUG
  cerr << "VariableTransformation::set_constrain this=" << this << " function=" << (void*) func;
  if (function)
    cerr << " " << func->get_name();
  cerr << endl;
#endif
  
  function[index] = func;
  chain.set_constraint (index, func);
}

unsigned VariableTransformation::get_index (unsigned jconstraint)
{
  auto it = function.begin();
  unsigned j = 0;
  while (j < jconstraint && it != function.end())
  {
    j++;
    it++;
  }

  if (j != jconstraint)
    throw Error (InvalidParam, "VariableTransformation::get_index",
          "jconstraint=%u >= nconstraint=%u", jconstraint, function.size());

  return it->first;
}

void VariableTransformation::set_argument (const Argument& argset)
{
  if ( argset.arguments.size() != function.size() )
    throw Error (InvalidState, "VariableTransformation::set_argument",
                 "number of parameter sets = %d does not equal number of functions = %d",
                 argset.arguments.size(), function.size() );

  for (auto arg : argset.arguments)
  {
    // arg.first is the parameter index
    auto f = function.at( arg.first );

    // arg.second is the vector of arguments for each abscissa
    unsigned nargs = arg.second.size();

    for (unsigned iarg=0; iarg < nargs; iarg++)
      f->set_abscissa_value (iarg, arg.second[iarg]);
  }

  pre_correction.set_value (argset.pre_correction);
  post_correction.set_value (argset.post_correction);
}

std::string Calibration::axis_value_to_string(const VariableTransformation::Argument& vtarg)
{
  string result;
  string space;  // no space before the first arg

  for (auto& arg: vtarg.arguments)
  {
    result += space + tostring(arg.first) + ":";
    for (unsigned i=0; i < arg.second.size(); i++)
    {
      if (i > 0)
        result += ",";
      result += tostring(arg.second[i]);
    }
    space = " ";  // space before subsequent args
  }
  return result;
}
