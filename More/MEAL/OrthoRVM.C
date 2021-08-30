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

  dPsi_dphi = new ScalarParameter;
  dPsi_dphi->set_value_name ("slope");

  atanh_cos_zeta = new ScalarParameter;
  atanh_cos_zeta->set_value_name ("atcz");
  
  /*
    The original RVM sign convention for PA is opposite to that of the IAU.
    See Everett & Weisberg (2001; ApJ 553:341) for more details.

    This is achieved here by reversing the sign of phi.
  */

  ScalarMath lon = *longitude - *magnetic_meridian;

  ScalarMath y = sin(lon);

  ScalarMath x = 1.0 / *dPsi_dphi + tanh(*atanh_cos_zeta) * (cos(lon) - 1.0);

  set_atan_Psi (y, x);
}

//! colatitude of line of sight with respect to spin axis
void MEAL::OrthoRVM::set_line_of_sight (double zeta)
{
  double cos_zeta = std::cos(zeta);
  double hyp = std::atanh (cos_zeta);
  atanh_cos_zeta->set_param (0, hyp);
}

Estimate<double> MEAL::OrthoRVM::get_line_of_sight () const
{
  ScalarMath cos_zeta = tanh( *atanh_cos_zeta );
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

