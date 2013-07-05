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
  if (!correlation)
    correlation = new ComplexCorrelation;

  if (!modeA)
  {
    modeA = new JonesSpinor;
    modeA->set_model (new SingularCoherency);
  }

  if (!modeB)
  {
    modeB = new JonesSpinor;
    modeB->set_model (new SingularCoherency);
  }

  SpinorJones* cross = new SpinorJones;
  cross->set_spinorA( modeA );
  cross->set_spinorB( modeB );

  Complex2* term = product( cast<Complex2> (correlation), (Complex2*) cross );
  Complex2* adjoint = convert( term, herm );

  wrap( sum (term, adjoint) );
}


MEAL::CrossCoherency::CrossCoherency ()
{
}

//! Copy constructor
MEAL::CrossCoherency::CrossCoherency (const CrossCoherency& copy)
{
  correlation = copy.correlation->clone();
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

void MEAL::CrossCoherency::set_modeA (SingularCoherency* A)
{
  if (!modeA)
    init ();

  modeA->set_model (A);
}

void MEAL::CrossCoherency::set_modeB (SingularCoherency* B)
{
  if (!modeB)
    init ();

  modeB->set_model (B);
}

void MEAL::CrossCoherency::set_correlation (Complex* c)
{
  correlation = c;
  init ();
}

//! Return the name of the class
string MEAL::CrossCoherency::get_name () const
{
  return "CrossCoherency";
}

