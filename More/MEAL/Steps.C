/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Steps.h"

using namespace std;

MEAL::Steps::Steps ()
  : parameters (this)
{
  current_step = -1;
}

//! Copy constructor
MEAL::Steps::Steps (const Steps& copy)
  : parameters (this)
{
  operator = (copy);
}

//! Assignment operator
MEAL::Steps& MEAL::Steps::operator = (const Steps& copy)
{
  parameters = copy.parameters;
  step = copy.step;
  current_step = copy.current_step;

  return *this;
}

//! Return the name of the class
string MEAL::Steps::get_name () const
{
  return "Steps";
}

//! Add a step at the given point on the abscissa
void MEAL::Steps::add_step (double x)
{
  unsigned size = step.size();
  assert ( size == parameters.get_nparam() );

  step.resize( size + 1 );
  parameters.resize( size + 1 );

  unsigned i=0;
  for (; i < size; i++)
    if (step[i] > x)
      break;

  double temp_step = step[i];
  double temp_parameter = parameters.get_param(i);

  step[i] = x;
  parameters.set_param(i, 0.0);

  // shift the rest of the values to the right
  for (i++; i < size+1; i++) {
    std::swap (temp_step, step[i]);
    parameters.swap_param(i, temp_parameter);
  }
}

//! Set the abscissa value
void MEAL::Steps::set_abscissa (double value)
{
  int nstep = step.size();
  int istep = nstep - 1;

  while (istep >= 0) {
    if (step[istep] < value)
      break;
    istep --;
  }

  if (istep >= 0)
    Univariate<Scalar>::set_abscissa (step[istep]);

  current_step = istep;
}


//! Return the value (and gradient, if requested) of the function
void MEAL::Steps::calculate (double& result, std::vector<double>* grad)
{
  if (step.size() == 0)
    throw Error (InvalidState, "MEAL::Steps::calculate",
		 "no steps and no free parameters");

  if (current_step)
    throw Error (InvalidState, "MEAL::Steps::calculate",
		 "current step unknown or invalid");

  result = parameters.get_param (current_step);

  if (grad) {
    grad->resize (get_nparam());
    for (unsigned ig=0; ig<get_nparam(); ig++)
      (*grad)[ig] = 0.0;
    (*grad)[current_step] = 1.0;
  }

  if (verbose) {
    cerr << "MEAL::Steps::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "MEAL::Steps::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}
