#include "Calibration/ScalarHypSine.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarHypSine::get_name () const
{
  return "ScalarHypSine";
}

//! The function
double Calibration::ScalarHypSine::function (double x) const
{
  return sinh(x); 
}

//! And its first derivative
double Calibration::ScalarHypSine::dfdx (double x) const
{ 
  return cosh(x);
}
