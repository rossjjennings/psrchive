/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MEAL/ScaledVonMises.h"
#include "MEAL/InverseRule.h"
#include "MEAL/ScalarBesselI0.h"
#include "MEAL/ScalarConstant.h"

using namespace std;
 
#if HAVE_GSL

#include <gsl_sf_bessel.h>

double variance (double kappa)
{
  return 1 - gsl_sf_bessel_I1 (kappa) / gsl_sf_bessel_I0 (kappa);
}

// d/dx((I_1(x))/(I_0(x))) = (I_0(x)^2+I_2(x) I_0(x)-2 I_1(x)^2)/(2 I_0(x)^2)
double dvariance_dkappa (kappa)
{
  double I0 = gsl_sf_bessel_I0 (kappa);
  double I1 = gsl_sf_bessel_I1 (kappa);
  double I2 = gsl_sf_bessel_In (2,kappa);

  return (2*I1*I1 - I0*I2 - I0*I0) / 2*(I0*I0);
}

#else

// first order approximation to I1(k)/I0(k) from Wolfram Alpha
double variance (double kappa)
{
  return 1.0 - kappa/2.0 + pow(kappa,3)/16.0 - pow(kappa,5)/96.0;
}

double dvariance_dkappa (double kappa)
{
  return -0.5 + pow(kappa,2)*3/16.0 - pow(kappa,4)*5/96.0;
}

#endif

// Uses Newton-Raphson method to solve for kappa, given variance
double kappa (double var)
{
  /* use the linear behaviour around erf(0) to get a good first guess */
  double guess = var;
  double dx = 0;
  int gi = 0;
  int gmax = 50;

  if (var < 0 || var > 2)
    throw Error (InvalidState, "kappa", "invalid variance=%f", var);

  for (gi=0; gi<gmax; gi++) 
  {
    dx = (variance(guess) - var) / dvariance_dkappa (guess);
    guess -= dx;
    if (fabs (dx) <= fabs(guess)*1e-10)
      return guess;
  }
  fprintf (stderr, "kappa: maximum iterations exceeded - %lf error\n", dx);
  return guess;
}

void
MEAL::ScaledVonMises::init()
{
  ScalarArgument* x = new ScalarArgument; 
  height.set_value_name("height");
  centre.set_value_name("centre"); 
  concentration.set_value_name("concentration");

  ScalarConstant *minus_one = new ScalarConstant(-1.0);
  ScalarMath result = exp ((cos(ScalarMath(x)-centre)+*minus_one)*concentration);

  if (log_height)
      result *= exp( height );
  else
      result *= height;

  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);
  copy_univariate_policy (x);
}

MEAL::ScaledVonMises::ScaledVonMises (bool _log_height) 
{
    log_height = _log_height;
    init ();
}

//! Copy constructor
MEAL::ScaledVonMises::ScaledVonMises (const ScaledVonMises& copy)
{
  log_height = copy.log_height;
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::ScaledVonMises&
MEAL::ScaledVonMises::operator = (const ScaledVonMises& copy)
{
  if (copy.log_height != this->log_height)
  {
      log_height = copy.log_height;
      init ();
  }

  Univariate<Scalar>::operator = (copy);
  return *this;
}
  
 

//! Set the centre
void MEAL::ScaledVonMises::set_centre (const Estimate<double>&centre_)
{
  centre.set_value(centre_);
}

//! Get the centre
Estimate<double> MEAL::ScaledVonMises::get_centre () const
{
  return centre.get_value ();
}

//! Set the concentration
void MEAL::ScaledVonMises::set_concentration (const Estimate<double>& concentration_)
{
  concentration.set_value (concentration_);
}

//! Get the concentration
Estimate<double> MEAL::ScaledVonMises::get_concentration () const
{
  return concentration.get_value ();
} 

//! Set the width
void MEAL::ScaledVonMises::set_width (const double width)
{
  concentration.set_value ( kappa(width*width) );
}

//! Get the width
double MEAL::ScaledVonMises::get_width () const
{
  double kappa = concentration.get_value().val;

  return ::sqrt(variance(kappa));
} 

//! Set the height
void MEAL::ScaledVonMises::set_height (const Estimate<double>& height_)
{
    if (log_height)
	height.set_value (log(height_));
    else
	height.set_value (height_);
}

//! Get the height
Estimate<double> MEAL::ScaledVonMises::get_height () const
{
    if (log_height)
	return exp(height.get_value());
    else
	return height.get_value();
}


std::string 
MEAL::ScaledVonMises::get_name() const
{
  return "ScaledVonMises";
}

// void MEAL::ScaledVonMises::parse (const string& line)
// {
//   Function::parse(line); // avoid using inherited GroupRule::parse()
// }

