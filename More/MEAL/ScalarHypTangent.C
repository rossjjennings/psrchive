#include "Calibration/ScalarHypTangent.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarHypTangent::get_name () const
{
  return "ScalarHypTangent";
}

//! The function
double Calibration::ScalarHypTangent::function (double x) const
{
  return tanh(x); 
}

//! And its first derivative
double Calibration::ScalarHypTangent::dfdx (double x) const
{ 
  double coshx = cosh(x);
  return 1.0/(coshx*coshx);
}
