/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/OrthogonalModes.h"
#include "MEAL/ModeCoherency.h"
#include "MEAL/UnitTangent.h"

#include "MEAL/NegationRule.h"
#include "MEAL/SumRule.h"

using namespace std;

void MEAL::OrthogonalModes::init ()
{
  ModeCoherency* A = new ModeCoherency;
  modeA = A;
  ModeCoherency* B = new ModeCoherency;
  modeB = B;
  
  B->set_axis( negation(A->get_axis()) );

  SumRule<Complex2>* sum = new SumRule<Complex2>;
  sum->add_model( A );
  sum->add_model( B );

  wrap (sum);
}

MEAL::OrthogonalModes::OrthogonalModes ()
{
  init ();
}

//! Copy constructor
MEAL::OrthogonalModes::OrthogonalModes (const OrthogonalModes& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::OrthogonalModes&
MEAL::OrthogonalModes::operator = (const OrthogonalModes& that)
{
  if (this == &that)
    return *this;

  modeA->copy( that.modeA );
  modeB->copy( that.modeB );

  return *this;
}

//! Destructor
MEAL::OrthogonalModes::~OrthogonalModes ()
{
}

//! Return the name of the class
string MEAL::OrthogonalModes::get_name () const
{
  return "OrthogonalModes";
}
