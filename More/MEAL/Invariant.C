/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Invariant.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarMath.h"

void MEAL::Invariant::init ()
{
  for (unsigned ipol=0; ipol<input.size(); ipol++)
    input[ipol] = *(new MEAL::ScalarParameter);

  result = input.invariant();
}

MEAL::Invariant::Invariant ()
{
  init ();
}

MEAL::Invariant::Invariant (const Invariant&)
{
  init ();
}

MEAL::Invariant::~Invariant ()
{

}

MEAL::Invariant&
MEAL::Invariant::operator = (const Invariant&)
{
  return *this;
}

void MEAL::Invariant::set_Stokes (const Stokes<Estimate<float> >& stokes)
{
  Stokes<Estimate<double> > temp = stokes;
  set_Stokes (temp);
}

void MEAL::Invariant::set_Stokes (const Stokes<Estimate<double> >& stokes)
{
  bias = stokes[0].get_variance();

  for (unsigned ipol=0; ipol<stokes.size(); ipol++) {
    input[ipol].get_expression()->set_Estimate( 0, stokes[ipol] );
    if (ipol)
      bias -= stokes[ipol].get_variance();
  }
}

//! Get the invariant interval
Estimate<double> MEAL::Invariant::get_invariant () const
{
  return result.get_Estimate();
}

//! Get the estimated bias due to measurement error
double MEAL::Invariant::get_bias () const
{
  return bias;
}

