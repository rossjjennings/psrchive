/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Function.h"

bool MEAL::Function::verbose = false;
bool MEAL::Function::very_verbose = false;
bool MEAL::Function::check_zero = false;
bool MEAL::Function::check_variance = false;

MEAL::Function::Function ()
{
#ifdef _DEBUG
  cerr << "MEAL::Function default constructor" << endl;
#endif
  evaluation_changed = true;
}

MEAL::Function::Function (const Function& model)
{
#ifdef _DEBUG
  cerr << "MEAL::Function copy constructor" << endl;
#endif

  parameter_policy = model.parameter_policy->clone (this);

  if (model.argument_policy)
    argument_policy = model.argument_policy->clone (this);

  evaluation_changed = true;
}

MEAL::Function::~Function ()
{
#ifdef _DEBUG
  cerr << "MEAL::Function::~Function" << endl;
#endif
}

MEAL::Function& MEAL::Function::operator = (const Function& model)
{
  copy (&model);
  return *this;
}

void MEAL::Function::copy (const Function* model)
{
#ifdef _DEBUG
  cerr << "MEAL::Function::copy" << endl;
#endif

  if (this == model)
    return;

  unsigned nparam = get_nparam ();

  if (nparam != model->get_nparam())
    throw Error (InvalidParam, "MEAL::Function::operator =",
		 "nparam=%d != copy.nparam=%d", nparam, model->get_nparam());

  for (unsigned iparam=0; iparam<nparam; iparam++) {

    set_param( iparam, model->get_param(iparam) );
    set_variance( iparam, model->get_variance(iparam) );
    set_infit( iparam, model->get_infit(iparam) );

  }

  // name = model->name;

  set_evaluation_changed();
}

//! Clone
MEAL::Function* MEAL::Function::clone () const
{
  throw Error (InvalidState, "MEAL::Function::clone", "not implemented" );
}

void MEAL::Function::copy_parameter_policy (const Function* function)
{
  parameter_policy = function->parameter_policy;
}

void MEAL::Function::set_parameter_policy (ParameterPolicy* policy)
{
  parameter_policy = policy;
  parameter_policy->context = this;
}

Estimate<double> MEAL::Function::get_Estimate (unsigned index) const
{
  return Estimate<double> ( get_param(index), get_variance(index) );
}

void MEAL::Function::set_Estimate (unsigned index,
				   const Estimate<double>& estimate)
{
  set_param( index, estimate.val );
  set_variance( index, estimate.var );
}

