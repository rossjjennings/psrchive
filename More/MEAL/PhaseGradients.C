/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "MEAL/PhaseGradients.h"
#include "MEAL/OneParameter.h"
#include <cassert>

void MEAL::PhaseGradients::init ()
{
  for (unsigned i=0; i<get_nparam(); i++)
    parameters.set_param_name (i, "phi_" + tostring(i));

  islope = 0;
}

MEAL::PhaseGradients::PhaseGradients (unsigned ncoef)
  : parameters (this, ncoef)
{
  init ();
}

//! Copy constructor
MEAL::PhaseGradients::PhaseGradients (const PhaseGradients& copy)
  : parameters (this)
{
  operator = (copy);
}

//! Assignment operator
MEAL::PhaseGradients& 
MEAL::PhaseGradients::operator = (const PhaseGradients& copy)
{
  if (&copy == this)
    return *this;

  parameters = copy.parameters;
  islope = copy.islope;

  return *this;
}

//! Clone operator
MEAL::PhaseGradients* MEAL::PhaseGradients::clone () const
{
  return new PhaseGradients( *this );
}

//! Set the current phase gradient
void MEAL::PhaseGradients::set_islope (unsigned i)
{
  if (i == islope)
    return;

  assert (i < parameters.get_nparam());
  islope = i;
  set_evaluation_changed ();
}

//! Add another slope to the set
void MEAL::PhaseGradients::add_slope ()
{
  parameters.resize( parameters.get_nparam() + 1 );
  init ();
  islope = parameters.get_nparam() - 1;
}

//! Get the number of slopes
unsigned MEAL::PhaseGradients::get_nslope () const
{
  return parameters.get_nparam();
}

//! Return the name of the class
string MEAL::PhaseGradients::get_name () const
{
  return "PhaseGradients";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
void MEAL::PhaseGradients::calculate (Jones<double>& result,
				      vector<Jones<double> >* grad)
{
  double x = get_abscissa();

  double phase = get_param(islope) * x;

  if (verbose)
    cerr << "MEAL::PhaseGradients::calculate phase=" << phase << endl;

  double cos_phase = cos(phase);
  double sin_phase = sin(phase);

  result = complex<double>(cos_phase, sin_phase);

  if (grad)
  {
    grad->resize (get_nparam());
    for (unsigned i=0; i<grad->size(); i++)
      (*grad)[i] = 0.0;
    (*grad)[islope] = x * complex<double>(-sin_phase, cos_phase);
    
    if (verbose)
      cerr << "MEAL::PhaseGradients::calculate gradient" << endl
	   << "   " << (*grad)[islope] << endl;
  }
  
}

