#include "Calibration/ScalarAbsolute.h"
#include <math.h>

//! Return the name of the class
string Calibration::ScalarAbsolute::get_name () const
{
  return "ScalarAbsolute";
}

//! The function
double Calibration::ScalarAbsolute::function (double x) const
{
  return fabs(x); 
}

//! And its first derivative
double Calibration::ScalarAbsolute::dfdx (double x) const
{ 
  if (x < 0)
    return -1;
  else
    return 1;
}
