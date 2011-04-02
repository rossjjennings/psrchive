/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarErrorFunction.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarErrorFunction::get_name () const
{
  return "ScalarErrorFunction";
}

//! The function
double MEAL::ScalarErrorFunction::function (double x) const
{
  return erf(x); 
}

//! And its first derivative
double MEAL::ScalarErrorFunction::dfdx (double x) const
{ 
  static const double scale = 2.0/sqrt(M_PI);
  return scale * exp(-x*x);
}
