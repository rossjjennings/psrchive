#include "Calibration/ScalarHypCosine.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarHypCosine::get_name () const
{
  return "ScalarHypCosine";
}

//! The function
double Calibration::ScalarHypCosine::function (double x) const
{
  return cosh(x); 
}

//! And its first derivative
double Calibration::ScalarHypCosine::dfdx (double x) const
{ 
  return sinh(x);
}
