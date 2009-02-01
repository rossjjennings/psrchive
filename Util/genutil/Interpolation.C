/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Interpolate.h"

using namespace std;

//! Default constructor
Interpolate::Interpolate ()
{
  xa = ya = 0;
  size = 0;
  interp = 0;
  acc = 0;
}

//! Destructor
Interpolate::~Interpolate ()
{
  destroy ();
}

//! Initialize interpolation object
void Interpolate::init (const std::vector<double>& x, 
			const std::vector<double>& y)
{
  destroy ();

  assert (x.size() == y.size());

  xa = &(x[0]);
  ya = &(y[0]);
  size = x.size();

  assert (xa);
  assert (ya);
  assert (size);

  interp = gsl_interp_alloc (gsl_interp_cspline, size);
  gsl_interp_init (interp, xa, ya, size);

  acc = gsl_interp_accel_alloc ();
}

void Interpolate::destroy ()
{
  if (acc)
    gsl_interp_accel_free (acc);
  if (interp)
    gsl_interp_free (interp);
}

//! Evaluate at the given abscissa
double Interpolate::eval (double x)
{
  assert (interp);
  assert (acc);

  return gsl_interp_eval (interp, xa, ya, x, acc);
}

