/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/OrthoRVM.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"

using namespace std;

void MEAL::OrthoRVM::init ()
{
  if (verbose)
    cerr << "MEAL::OrthoRVM::init" << endl;

  inverse_slope = new ScalarParameter;
  inverse_slope->set_value_name ("kappa");

  line_of_sight = new ScalarParameter;
  line_of_sight->set_value_name ("zeta");
  
  /*
    The original RVM sign convention for PA is opposite to that of the IAU.
    See Everett & Weisberg (2001; ApJ 553:341) for more details.

    This is achieved here by reversing the sign of phi.
  */

  ScalarMath lon = *longitude - *magnetic_meridian;

  ScalarMath y = sin(lon);

  ScalarMath x = *inverse_slope + cos(*line_of_sight) * (cos(lon)-1.0);

  set_atan_Psi (y, x);
}

MEAL::OrthoRVM::OrthoRVM ()
{
  init ();
}

//! Copy constructor
MEAL::OrthoRVM::OrthoRVM (const OrthoRVM& rvm)
{
  init ();
  operator = (rvm);
}

//! Assignment operator
MEAL::OrthoRVM&
MEAL::OrthoRVM::operator = (const OrthoRVM& copy)
{
  Univariate<Scalar>::operator = (copy);
  return *this;
}

MEAL::OrthoRVM::~OrthoRVM ()
{
}

//! Return the name of the class
string MEAL::OrthoRVM::get_name () const
{
  return "OrthoRVM";
}

