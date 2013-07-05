/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Spinor.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Spinor::Name = "Spinor";

MEAL::Spinor* MEAL::Spinor::clone () const
{
  throw Error (InvalidState, "MEAL::Spinor::clone", "not implemented");
}
