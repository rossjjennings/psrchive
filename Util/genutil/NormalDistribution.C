/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "NormalDistribution.h"
#include <math.h>

static const double norm = 1.0 / sqrt (2.0 * M_PI);

//! Get the value of the probability density at x
double NormalDistribution::density (double x)
{
  return norm * exp (-0.5*(x*x));
}

//! Get the value of the distribution function at x
/*! The distribution, \f$ D(x) = \int_{-\inf}^x P(t) dt \f$ */
double NormalDistribution::cumulative_distribution (double x)
{
  return 0.5 * (1.0 + erf (x * M_SQRT1_2));
}

//! Get the first moment of the distribution averaged from 0 to x
/*! i.e. \f$ \int_{-inf}^x t P(t) dt \over D(x) \f$ */
double NormalDistribution::cumulative_mean (double x)
{
  return - density (x) / cumulative_distribution(x);
}

//! Get the second moment of the distribution averaged from 0 to x
/*! i.e. \f$ \int_{-inf}^x t^2 P(t) dt \over D(x) \f$ */
double NormalDistribution::cumulative_moment2 (double x)
{
  return 1.0 - x * density(x) / cumulative_distribution(x);
}

