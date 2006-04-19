/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/NormalizeStokes.h"
#include "MEAL/ScalarMath.h"

void MEAL::NormalizeStokes::init ()
{
  invariant = new Invariant;

  ScalarMath invint = sqrt( invariant->get_correct_result() );

  output = invariant->get_input() / invint;
}

MEAL::NormalizeStokes::NormalizeStokes ()
{
  init ();
}

MEAL::NormalizeStokes::NormalizeStokes (const NormalizeStokes&)
{
  init ();
}

MEAL::NormalizeStokes::~NormalizeStokes ()
{

}

MEAL::NormalizeStokes&
MEAL::NormalizeStokes::operator = (const NormalizeStokes&)
{
  return *this;
}

void
MEAL::NormalizeStokes::normalize (Stokes<Estimate<float> >& stokes)
{
  Stokes<Estimate<double> > temp = stokes;
  normalize (temp);
  stokes = temp;
}

//! Set the scale to be used during conversion
void MEAL::NormalizeStokes::normalize (Stokes<Estimate<double> >& stokes)
{
  invariant->set_Stokes (stokes);

  Estimate<double> invint = stokes.invariant();

  if ( invint.val < stokes[0].var )
    throw Error (InvalidParam, "MEAL::NormalizeStokes::normalize",
		 "invariant=%lf < variance=%lf", invint.val, stokes[0].var);

  Stokes< Estimate<double> > normalized;

  for (unsigned ipol=0; ipol<stokes.size(); ipol++)
    normalized[ipol] = output[ipol].get_Estimate();

  stokes = normalized;
}
