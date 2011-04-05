/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/CrossCoherency.h"
#include "MEAL/OrthogonalModes.h"
#include "MEAL/ModeCoherency.h"
#include "MEAL/UnitTangent.h"

#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"

using namespace std;

void MEAL::CrossCoherency::init ()
{
  composite.map (modes);

  setup (corrA1);
  setup (corrB1);
  setup (corrC2);
  setup (corrC3);
}

void MEAL::CrossCoherency::setup (Project<Scalar>& corr)
{
  corr = erf( *(new ScalarParameter) ).get_expression();
  composite.map (corr);
}

MEAL::CrossCoherency::CrossCoherency (OrthogonalModes* _modes)
  : composite (this)
{
  modes = _modes;
  init ();
}

//! Copy constructor
MEAL::CrossCoherency::CrossCoherency (const CrossCoherency& copy)
  : composite (this)
{
  modes = new OrthogonalModes (*(copy.modes));
  init ();

  operator = (copy);
}

//! Assignment operator
MEAL::CrossCoherency&
MEAL::CrossCoherency::operator = (const CrossCoherency& that)
{
  if (this == &that)
    return *this;

  modes->copy( that.modes );

  corrA1->copy( that.corrA1 );
  corrB1->copy( that.corrB1 );
  corrC2->copy( that.corrC2 );
  corrC3->copy( that.corrC3 );

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


//! Calculate the Jones matrix and its gradient
void MEAL::CrossCoherency::calculate (Jones<double>& result,
				      vector<Jones<double> >* grad)
{
  ModeCoherency* A = dynamic_cast<ModeCoherency*>( modes->get_modeA() );
  ModeCoherency* B = dynamic_cast<ModeCoherency*>( modes->get_modeB() );
}
