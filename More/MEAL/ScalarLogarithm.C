#include "Calibration/ScalarLogarithm.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarLogarithm::get_name () const
{
  return "ScalarLogarithm";
}

//! The function
double Calibration::ScalarLogarithm::function (double x) const
{
  if (x < 0.0)
    throw Error (InvalidState, "Calibration::ScalarLogarithm::function",
		 "x < 0");
  return log(x); 
}

//! And its first derivative
double Calibration::ScalarLogarithm::dfdx (double x) const
{ 
  if (x < 0.0)
    throw Error (InvalidState, "Calibration::ScalarLogarithm::dfdx",
		 "x < 0");
  return 1/x;
}
