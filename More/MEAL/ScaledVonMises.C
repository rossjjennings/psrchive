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
#include "MEAL/ScalarConstant.h"

using namespace std;
 
/*
  besrat and vkappa Fortran implementations from

  "Evaluation and Inversion of the Ratios of Modified Bessel
  Functions, I1(x) /I0 (x) and I 1.5(x)/ I0.5(x)"

  Hill, Geoffrey W. (1981)
  ACM Transactions on Mathematical Software (TOMS), vol. 7, pp. 199 - 208
*/

// defined in besrat.f
#define F77_besrat F77_FUNC(besrat,BESRAT)
extern "C" float F77_besrat (float* kappa);

float variance (float kappa)
{
  return 1 - F77_besrat(&kappa);
}

// defined in vkappa.f
#define F77_vkappa F77_FUNC(vkappa,VKAPPA)
extern "C" float F77_vkappa (float* kappa);

// defined in bessi0.c
extern "C" double bessi0 (double);


float kappa (float variance)
{
  float arg = 1 - variance;
  return F77_vkappa(&arg);
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
void MEAL::ScaledVonMises::set_concentration (const Estimate<double>& kappa)
{
  concentration.set_value (kappa);
}

//! Get the concentration
Estimate<double> MEAL::ScaledVonMises::get_concentration () const
{
  return concentration.get_value ();
} 

//! Set the width in radians
void MEAL::ScaledVonMises::set_width (double width)
{
  double circular_variance = width*width;
  double _kappa = kappa(circular_variance);

#if 1
  cerr << "MEAL::ScaledVonMises::set_width"
    " width=" << width <<
    " var=" << circular_variance <<
    " kappa=" << _kappa << endl;
#endif

  concentration.set_value ( _kappa );
}

//! Get the width in radians
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


double MEAL::ScaledVonMises::get_area () const
{
  double _kappa = get_concentration().get_value();

  // cerr << "MEAL::ScaledVonMises::get_area concentration=" << _kappa << endl;
  
  return 2.0 * M_PI * ::bessi0(_kappa) / ::exp(_kappa);
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

