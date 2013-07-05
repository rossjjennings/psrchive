/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/JonesCloude.h"
#include "Cloude.h"

MEAL::JonesCloude::JonesCloude (Complex2* xform)
{
  if (xform)
    set_transformation (xform);
}

std::string MEAL::JonesCloude::get_name () const
{
  std::string name = "JonesCloude";
  if (has_model())
    name += "[" + get_model()->get_name() + "]";
  return name;
}

//! Calculate the Cloude matrix and its gradient
void MEAL::JonesCloude::calculate (Result& result,
				   std::vector<Result>* grad)
{
  std::vector< Jones<double> > jones_grad;
  std::vector< Jones<double> >* jones_grad_ptr = 0;
  
  if (grad)
    jones_grad_ptr = &jones_grad;

  Jones<double> jones_result = get_model()->evaluate (jones_grad_ptr);

  result = coherence ( jones_result );

  if (!grad)
    return;

  for (unsigned i=0; i<grad->size(); i++)
    (*grad)[i] = coherence ( jones_result, jones_grad[i] );
}

