/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarAtanc.h"
#include <math.h>

using namespace std;

// compute atan(z)/z, where z = pi*x/2, using the Mclaurin series of atan(z)
double atanc (double x, double* derivative, double* datancc)
{
  double z = x * M_PI * 0.5;

  if (fabs(z) > 0.5)
  {
    double result = atan (z) / z;

    if (derivative)
    {
      *derivative = 1.0 / (z*(1+z*z)) - result / z;
      *derivative *= M_PI * 0.5;  // dz/dx
    }

    if (datancc)
      *datancc = 1.0 / (z*z*(1+z*z)) - result / (z*z);

    return result;
  }

  // for small values of z, use the McLaurin series of atan(z)

  double result = 0.0;

  if (derivative)
    *derivative = 0.0;

  if (datancc)
    *datancc = -2.0/3.0;

  double powz_n = z;

  double sign = -1.0;
  double N = 2.0;

  while (fabs(powz_n) > 1e-18)
  {
    // If worried about rounding error, could 
    //
    // powz_n = pow(z,N-1);

    if (derivative)
      *derivative += sign * powz_n * N / (N+1);

    powz_n *= z;

    result += sign * powz_n / (N+1);

    N += 2;
    sign *= -1.0;

    if (datancc)
      *datancc += sign * powz_n * N / (N+1);

    powz_n *= z;

  }

  if (derivative)
    *derivative *= M_PI * 0.5;  // dz/dx

  return 1 + result;
}

//! Return the name of the class
string MEAL::ScalarAtanc::get_name () const
{
  return "ScalarAtanc";
}

//! The function
double MEAL::ScalarAtanc::function (double x) const
{
  return atanc (x); 
}

//! The partial derivative with respect to arg1
double MEAL::ScalarAtanc::dfdx (double x) const
{
  return 1.0/(1+x*x);
}
