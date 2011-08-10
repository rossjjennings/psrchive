/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/CrossCoherency.h"

#include "MEAL/JonesSpinor.h"
#include "MEAL/ComplexCorrelation.h"
#include "MEAL/SpinorJones.h"

#include "MEAL/ProductRule.h"
#include "MEAL/SumRule.h"
#include "MEAL/Cast.h"
#include "MEAL/UnaryConvert.h"

using namespace std;

void MEAL::CrossCoherency::init ()
{
  correlation = new ComplexCorrelation;
  modeA = new JonesSpinor;
  modeB = new JonesSpinor;

  SpinorJones* cross = new SpinorJones;
  cross->set_spinorA( modeA );
  cross->set_spinorB( modeB );

  Complex2* term = product( cast<Complex2> (correlation), (Complex2*) cross );
  Complex2* adjoint = convert( term, herm );

  wrap( sum (term, adjoint) );
}


MEAL::CrossCoherency::CrossCoherency ()
{
  init ();
}

//! Copy constructor
MEAL::CrossCoherency::CrossCoherency (const CrossCoherency& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::CrossCoherency&
MEAL::CrossCoherency::operator = (const CrossCoherency& that)
{
  if (this == &that)
    return *this;

  copy( &that );

  return *this;
}

//! Destructor
MEAL::CrossCoherency::~CrossCoherency ()
{
}

//! Return the name of the class
string MEAL::CrossCoherency::get_name () const
{
  return "CrossCoherency";
}

