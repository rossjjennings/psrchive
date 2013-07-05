/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/JonesMueller.h"
#include "Pauli.h"
#include <assert.h>

MEAL::JonesMueller::JonesMueller (Complex2* xform)
{
  if (xform)
    set_transformation (xform);
}

std::string MEAL::JonesMueller::get_name () const
{
  std::string name = "JonesMueller";
  if (has_model())
    name += "[" + get_model()->get_name() + "]";
  return name;
}


//! Calculate the Mueller matrix and its gradient
void MEAL::JonesMueller::calculate (Matrix<4,4,double>& result,
				    std::vector<Matrix<4,4,double> >* grad)
{
  std::vector< Jones<double> > jones_grad;
  std::vector< Jones<double> >* jones_grad_ptr = 0;
  
  if (grad)
    jones_grad_ptr = &jones_grad;

  Jones<double> jones_result = get_model()->evaluate (jones_grad_ptr);

  result = Mueller( jones_result );

  if (!grad)
    return;

  assert( grad->size() == jones_grad.size() );

  for (unsigned i=0; i<grad->size(); i++)
    (*grad)[i] = Mueller( jones_result, jones_grad[i] );
}

