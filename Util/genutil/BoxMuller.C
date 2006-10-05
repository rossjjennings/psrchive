/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "BoxMuller.h"
#include <math.h>

#include <r250.hpp> // from utils/third/taygeta

static R250 the_r250;

BoxMuller::BoxMuller (long seed)
{
  have_one_ready = false;
  one_ready = 0;
  if (seed)
    the_r250.seed(seed);
}

//! returns a random variable with a Gaussian distribution
float BoxMuller::operator () ()
{
  if (have_one_ready) {
    /* use value from previous call */
    have_one_ready = false;
    return one_ready;
  }

  float v1, v2, w;

  do {
    v1 = 2.0 * the_r250.ranf() - 1.0;
    v2 = 2.0 * the_r250.ranf() - 1.0;
    w = v1 * v1 + v2 * v2;
  } while ( w >= 1.0 );
  
  w = sqrt( (-2.0 * log( w ) ) / w );

  have_one_ready = true;
  one_ready = v2 * w;

  return v1 * w;
  
}
