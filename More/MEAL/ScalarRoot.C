#include "Calibration/ScalarRoot.h"
#include <math.h>

//
//
//
string Calibration::ScalarRoot::get_name () const
{
  return "ScalarRoot";
}

//
//
//
double Calibration::ScalarRoot::function (double x) const
{
  return sqrt(x); 
}

//
//
//
double Calibration::ScalarRoot::dfdx (double x) const
{ 
  return 0.5/sqrt(x);
}
