/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "normal.h"
#include <math.h>

/* Returns the standard normal probability function */
double normal_probability (double x)
{
  const double norm = 1.0 / sqrt (2.0 * M_PI);
  return norm * exp (-0.5*(x*x));
}

/* http://mathworld.wolfram.com/NormalDistributionFunction.html */
double normal_distribution (double x)
{
  return 0.5 * erf (x * M_SQRT1_2);
}

/* Eq. 9 at http://mathworld.wolfram.com/NormalDistribution.html */
double normal_cummulative (double x)
{
  return 0.5 + normal_distribution(x);
}

/* Calculating the expectation value of x^2 over the interval -t to t:

   <x^2> = \int_{-t}^{t} x^2 p(x) dx / \int_{-t}^{t} p(x) dx

   where p(x) = normal_probability(x) = 1/sqrt(2pi) exp(-x^2/2)

   For the numerator, integration by parts: \int u dv = uv - \int v du

   with

   u=x      dv=x p(x) dx
   du=dx    v=-p(x)
   
   <x^2> = [-t p(t) + Phi(t)] / Phi(t)

   where Phi(t) = \int_0^t p(x) dx
*/
double normal_moment2 (double x)
{
  return 1.0 - x * normal_probability(x) / normal_distribution(x);
}

