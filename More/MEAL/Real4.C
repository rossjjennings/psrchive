/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Real4.h"
#include "MEAL/Cached.h"

#include <complex>

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Real4::Name = "Real4";

MEAL::Real4::Real4 ()
{
  evaluation_policy = new Cached<Real4> (this);
}

MEAL::Real4::Real4 (const Real4& copy) : Function (copy)
{
  evaluation_policy = new Cached<Real4> (this);
}

MEAL::Real4& MEAL::Real4::operator = (const Real4& copy)
{
  Function::operator = (copy);
}

