/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ComplexRVM.h"
#include "MEAL/RotatingVectorModel.h"

#include "MEAL/Phase.h"
#include "MEAL/ChainParameters.h"

using namespace std;

void MEAL::ComplexRVM::init ()
{
  rvm = new RotatingVectorModel;

  ChainParameters<Complex>* chain = new ChainParameters<Complex>;

  // Set up a complex phase function with phase equal to RVM P.A.
  chain->set_model( new Phase<Complex> );
  chain->set_constraint( 0, rvm );

  
}

MEAL::ComplexRVM::ComplexRVM ()
{
  init ();
}

//! Copy constructor
MEAL::ComplexRVM::ComplexRVM (const ComplexRVM& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::ComplexRVM& MEAL::ComplexRVM::operator = (const ComplexRVM& copy)
{
  rvm->copy (copy.rvm);
  return *this;
}

MEAL::ComplexRVM::~ComplexRVM ()
{
}

//! Return the rotating vector model
MEAL::RotatingVectorModel* MEAL::ComplexRVM::get_rvm ()
{
  return rvm;
}

//! Return the name of the class
string MEAL::ComplexRVM::get_name () const
{
  return "ComplexRVM";
}

