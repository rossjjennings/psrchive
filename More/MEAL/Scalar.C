/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Scalar.h"

using namespace std;

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Scalar::Name = "Scalar";

MEAL::Scalar::Scalar ()
{
  if (verbose)
    cerr << "MEAL::Scalar ctor" << endl;
  evaluation_policy = default_evaluation_policy (this);
}

MEAL::Scalar::Scalar (const Scalar& copy) : Function (copy)
{
  if (verbose)
    cerr << "MEAL::Scalar copy ctor" << endl;
  evaluation_policy = default_evaluation_policy (this);
}

MEAL::Scalar& MEAL::Scalar::operator = (const Scalar& copy)
{
  Function::operator = (copy);
  return *this;
}

//! Clone
MEAL::Scalar* MEAL::Scalar::clone () const
{
  throw Error (InvalidState, "MEAL::Scalar::clone", "not implemented" );
}

void MEAL::Scalar::evaluate (Estimate<double>& value) const
{
  std::vector<double> gradient;

  value = evaluate (&gradient);

  unsigned nparam = get_nparam();

  if (gradient.size() != nparam)
    throw Error (InvalidState, "MEAL::Scalar::evaluate",
		 "gradient.size=%d != nparam=%d", gradient.size(), nparam);

  for (unsigned iparam=0; iparam<nparam; iparam++)
    value.var += gradient[iparam] * gradient[iparam] * get_variance(iparam);

}

//! Return the scalar value and its variance
Estimate<double> MEAL::Scalar::estimate () const
{
  Estimate<double> ret;
  evaluate (ret);
  return ret;
}

void MEAL::Scalar::copy_evaluation_policy (const Scalar* function)
{
  evaluation_policy = function->evaluation_policy;
}

