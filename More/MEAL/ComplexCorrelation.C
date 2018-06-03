/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ComplexCorrelation.h"
#include "MEAL/Parameters.h"

using namespace std;

MEAL::ComplexCorrelation::ComplexCorrelation ()
{
  Parameters* params = new Parameters (this, 2);
  params->set_name (0, "real");
  params->set_name (1, "imag");
}

static double c[] = { -1.0/3.0, 2.0/15.0, -17.0/315.0, 62.0/2835.0 };

double tanh_on_x (double x)
{
  double X=x*x;
  return 1.0 + c[0]*X + c[1]*X*X + c[2]*X*X*X + c[3]*(X*X)*(X*X);
}

double dtanch_on_x (double x)
{
  double X=x*x;
  return 2*c[0] + 4*c[1]*X + 6*c[2]*X*X + 8*c[3]*X*(X*X);
}

//! Calculate the Jones matrix and its gradient
void MEAL::ComplexCorrelation::calculate (Result& result,
					  vector<Result>* grad)
{
  std::complex<double> z ( get_param(0), get_param(1) );
  double mod_z = abs (z);

  /*
    use the hyperbolic tangent to "saturate" the magnitude of the complex
    correlation coefficient such that it remains less than unity
  */

  double tanch = 0.0;

  // Use the McLaurin Series of tanh(x)/x near zero
  double threshold = 1e-4;
  
  if (mod_z < threshold)
    tanch = tanh_on_x(mod_z);
  else
    tanch = tanh(mod_z)/mod_z;

  result = z * tanch;

  if (verbose)
    cerr << "MEAL::ComplexCorrelation::calculate z=" << result << endl;

  if (grad)
  {
    double dtanch_on_z = 0.0;
    if (mod_z < threshold)
    {
      dtanch_on_z = dtanch_on_x(mod_z);
    }
    else
    {
      double cz = cosh(mod_z);
      dtanch_on_z = (1/(cz*cz) - tanch) / (mod_z*mod_z);
    }

    (*grad)[0] = std::complex<double>( tanch, 0 )
      + z * dtanch_on_z * z.real();

    (*grad)[1] = std::complex<double>( 0, tanch )
      + z * dtanch_on_z * z.imag();
  }
}
