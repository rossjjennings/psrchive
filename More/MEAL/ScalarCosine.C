#include "Calibration/ScalarCosine.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarCosine::get_name () const
{
  return "ScalarCosine";
}

//! The function
double Calibration::ScalarCosine::function (double x) const
{
  return cos(x); 
}

//! And its first derivative
double Calibration::ScalarCosine::dfdx (double x) const
{ 
  return -sin(x);
}
