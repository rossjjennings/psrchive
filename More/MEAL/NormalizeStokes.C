/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/NormalizeStokes.h"
#include "MEAL/ScalarMath.h"

using namespace std;

void MEAL::NormalizeStokes::init ()
{
  invariant = new Invariant;
  other = new ScalarParameter;

  Reference::To<Scalar> temp = other.get();

  ScalarMath invint = sqrt( invariant->get_correct_result() + temp );

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

//! Set the other term in the denominator
void MEAL::NormalizeStokes::set_other (const Estimate<double>& value)
{
  other->set_value( value );
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

  Estimate<double> norm = ::invariant(stokes) + other->get_value();

#ifdef _DEBUG
  cerr << "MEAL::NormalizeStokes::normalize norm=" << norm << endl;
#endif
  
  if ( norm.val < stokes[0].var )
    throw Error (InvalidState, "MEAL::NormalizeStokes::normalize",
		 "norm=%lf < variance=%lf", norm.val, stokes[0].var);

  Stokes< Estimate<double> > normalized;

  for (unsigned ipol=0; ipol<stokes.size(); ipol++)
  {
    normalized[ipol] = output[ipol].get_Estimate();
    if (normalized[ipol].get_variance() < 0.0)
      throw Error (InvalidParam, "MEAL::NormalizeStokes::normalize",
                   "ipol=%d variance=%lf",
		   ipol, normalized[ipol].get_variance());
  }

  stokes = normalized;
}

void MEAL::NormalizeStokes::normalize (Stokes< Estimate<double> >& stokes,
                                       const Estimate<double>& total_squared_invariant)
{
  Estimate<double> invint_squared = ::invariant (stokes);

#ifdef _DEBUG
  cerr << "MEAL::NormalizeStokes::normalize invint=" << invint_squared
       << "tot_inv_squared=" << total_squared_invariant << endl;
#endif

  Estimate<double> remainder = total_squared_invariant;

  // subtract the square of the invariant of the input from the total invariant squared
  remainder.val -= invint_squared.val;
  remainder.var -= invint_squared.var;

#ifdef _DEBUG
  cerr << "MEAL::NormalizeStokes::normalize remaining tot_inv_squared=" << remainder << endl;
#endif

  set_other (remainder);
  normalize (stokes);
}
