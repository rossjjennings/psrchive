#include "Calibration/ScalarPower.h"
#include <math.h>

//
//
//
string Calibration::ScalarPower::get_name () const
{
  return "ScalarPower";
}

//
//
//
double Calibration::ScalarPower::function (double x, double y) const
{
  return pow (x, y);
}

//
//
//
double Calibration::ScalarPower::partial_arg1 (double x, double y) const
{
  // d/dx x^y = y x^(y-1)
  return y * pow (x, y-1.0);
}

//
//
//
double Calibration::ScalarPower::partial_arg2 (double x, double y) const
{
  // d/dy x^y = ln(x) x^y
  return log(x) * pow (x, y);
}
