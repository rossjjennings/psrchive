/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/JonesSpinor.h"
#include <assert.h>

using namespace std;

#define POLE_UNSET 2

MEAL::JonesSpinor::JonesSpinor ()
{
  pole = POLE_UNSET;
}

std::string MEAL::JonesSpinor::get_name () const
{
  return "JonesSpinor";
}

MEAL::Spinor::Result spinor (const Jones<double>& J, unsigned pole)
{
  MEAL::Spinor::Result result;

  double norm = sqrt( J(pole,pole).real() );
  result[pole] = norm;
  result[!pole] = J(!pole,pole)/norm;

  return result;
}

MEAL::Spinor::Result spinor (const Jones<double>& J, 
			     const Jones<double>& dJ,
			     unsigned pole)
{
  MEAL::Spinor::Result result;

  double norm = sqrt( J(pole,pole).real() );
  result[pole] = 0.5 * dJ(pole,pole)/norm;

  result[!pole] = dJ(!pole,pole)/norm 
    - 0.5 * dJ(pole,pole) * J(!pole,pole) / (norm * J(pole,pole).real() );

  return result;
}

unsigned get_pole (const Jones<double>& J)
{
  if (J(0,0).real() > J(1,1).real())
    return 0;
  else
    return 1;
}

//! Calculate the Mueller matrix and its gradient
void MEAL::JonesSpinor::calculate (Spinor::Result& result,
				   std::vector< Spinor::Result >* grad)
{
  std::vector< Jones<double> > jones_grad;
  std::vector< Jones<double> >* jones_grad_ptr = 0;
  
  if (grad)
    jones_grad_ptr = &jones_grad;

  Jones<double> jones_result = get_model()->evaluate (jones_grad_ptr);

  if (pole == POLE_UNSET)
    pole = get_pole (jones_result);

  result = spinor( jones_result, pole );

  if (!grad)
    return;

  assert( grad->size() == jones_grad.size() );

  for (unsigned i=0; i<grad->size(); i++)
    (*grad)[i] = spinor( jones_result, jones_grad[i], pole );

}

