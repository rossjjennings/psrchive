#include "Calibration/ScalarExponential.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarExponential::get_name () const
{
  return "ScalarExponential";
}

//! The function
double Calibration::ScalarExponential::function (double x) const
{
  return exp(x); 
}

//! And its first derivative
double Calibration::ScalarExponential::dfdx (double x) const
{ 
  return exp(x);
}
