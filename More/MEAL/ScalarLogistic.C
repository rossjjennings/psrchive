/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarLogistic.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarLogistic::get_name () const
{
  return "ScalarLogistic";
}

//! The function
double MEAL::ScalarLogistic::function (double x) const
{
  return 1.0 / (1.0 + exp(-x)); 
}

//! And its first derivative
double MEAL::ScalarLogistic::dfdx (double x) const
{ 
  double fx = function(x);
  return fx * (1.0 - fx);
}
