#include "Calibration/ScalarSine.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarSine::get_name () const
{
  return "ScalarSine";
}

//! The function
double Calibration::ScalarSine::function (double x) const
{
  return sin(x); 
}

//! And its first derivative
double Calibration::ScalarSine::dfdx (double x) const
{ 
  return cos(x);
}
