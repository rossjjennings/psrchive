//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Interpolate_h
#define __Pulsar_Interpolate_h

#include "Reference.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

#include <vector>

//! Interface to GSL interpolation routines
class Interpolate : public Reference::Able
{

public:

  //! Default constructor
  Interpolate ();

  //! Destructor
  virtual ~Interpolate ();

  //! Initialize interpolation object
  void init (const std::vector<double>& x, const std::vector<double>& y);

  //! Evaluate at the given abscissa
  double eval (double x);

protected:

  const double* xa;
  const double* ya;
  size_t size;

  gsl_interp* interp;
  gsl_interp_accel* acc;

  void destroy ();
};

#endif
