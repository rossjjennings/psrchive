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
  return exp (-0.5*(x*x)) / sqrt(2*M_PI);
}

/* Returns the standard normal cummulative distribution function */
double normal_cummulative (double x)
{
  return 0.5 * (1 + erf (x/sqrt(2.0)));
}

/* Returns the indefinite integral of the second moment of standard
   normal probability function */
double normal_moment2 (double x)
{
  return normal_cummulative(x) - 0.5 - normal_probability(x)*x;
}

