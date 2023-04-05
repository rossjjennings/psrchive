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

#include <cmath>

using namespace std;


void MEAL::OrthoRVM::init ()
{
  if (verbose)
    cerr << "MEAL::OrthoRVM::init" << endl;

  kappa = new ScalarParameter;
  kappa->set_value_name ("kappa");

  lambda = new ScalarParameter;
  lambda->set_value_name ("lambda");
  
  /*
    The original RVM sign convention for PA is opposite to that of the IAU.
    See Everett & Weisberg (2001; ApJ 553:341) for more details.

    This is achieved here by reversing the sign of the denominator, x.

    Note that dPsi_dphi = -sin(alpha)/sin(beta)
  */

  ScalarMath lon = *longitude - *magnetic_meridian;

  ScalarMath y = sin(lon);

  cos_zeta = *lambda / sqrt (*lambda * *lambda + 1.0);

  ScalarMath x = *kappa + cos_zeta * (cos(lon) - 1.0);

  set_atan_Psi (y, x);
}

//! colatitude of line of sight with respect to spin axis
void MEAL::OrthoRVM::set_line_of_sight (double zeta)
{
  lambda->set_param (0, 1.0/::tan(zeta));
}

Estimate<double> MEAL::OrthoRVM::get_line_of_sight () const
{
  return acos (cos_zeta.get_Estimate());
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

