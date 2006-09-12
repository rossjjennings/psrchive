/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/MuellerTransformation.h"
#include "MEAL/ProjectGradient.h"
#include "Pauli.h"

using namespace std;

MEAL::MuellerTransformation::MuellerTransformation ()
  : composite (this)
{
}

MEAL::MuellerTransformation::~MuellerTransformation ()
{
}

//! Return the name of the class
string MEAL::MuellerTransformation::get_name () const
{
  return "MuellerTransformation";
}

//! Set the transformation, \f$ M \f$
/*! This method unmaps the old transformation before mapping xform */
void MEAL::MuellerTransformation::set_transformation (Real4* xform)
{
  if (!xform)
    return;

  if (transformation) {
    if (verbose)
      cerr << "MEAL::MuellerTransformation::set_transformation"
	" unmap old transformation" << endl;
    composite.unmap (transformation, false);
  }

  transformation = xform;

  if (verbose)
    cerr << "MEAL::MuellerTransformation::set_transformation"
      " map new transformation" << endl;

  composite.map (transformation);
}

//! Get the transformation, \f$ J \f$
MEAL::Real4* MEAL::MuellerTransformation::get_transformation ()
{
  return transformation;
}

/*! This method unmaps the old input before mapping xform */
void MEAL::MuellerTransformation::set_input (Complex2* xform)
{
  if (!xform)
    return;

  if (input) {
    if (verbose)
      cerr << "MEAL::MuellerTransformation::set_input unmap old input" << endl;
    composite.unmap (input, false);
  }

  input = xform;

  if (verbose)
    cerr << "MEAL::MuellerTransformation::set_input"
      " map new input" << endl;

  composite.map (input);
}

//! Get the input, \f$ \rho \f$
MEAL::Complex2* MEAL::MuellerTransformation::get_input ()
{
  return input;
}

//! Returns \f$ \rho^\prime_j \f$ and its gradient
void MEAL::MuellerTransformation::calculate (Jones<double>& result,
					     std::vector<Jones<double> >* grad)
{
  if (verbose)
    cerr << "MEAL::MuellerTransformation::calculate" << endl;

  // gradient of transformation
  std::vector<Matrix<4,4,double> > xform_grad;
  std::vector<Matrix<4,4,double> > *xform_grad_ptr = 0;

  // gradient of input
  std::vector<Jones<double> > input_grad;
  std::vector<Jones<double> > *input_grad_ptr = 0;

  if (grad) {
    input_grad_ptr = &input_grad;
    xform_grad_ptr = &xform_grad;
  }

  // compute xform and partial derivatives with respect to xform parameters
  Matrix<4,4,double> xform = transformation->evaluate (xform_grad_ptr);

  // compute input and partial derivatives with respect to input parameters
  Jones<double> input_jones = input->evaluate (input_grad_ptr);

  // set the result
  result = transform (xform, input_jones);

  if (verbose)
    cerr << "MEAL::MuellerTransformation::evaluate result\n"
	 "   " << result << endl;

  if (!grad)
    return;

  // compute the partial derivatives

  unsigned igrad;

  // resize the gradient for the partial derivatives wrt all parameters
  grad->resize (get_nparam());
  for (igrad = 0; igrad<grad->size(); igrad++)
    (*grad)[igrad] = 0;

  std::vector<Jones<double> > grad_jones (xform_grad.size());

  // compute the partial derivatives wrt transformation parameters
  for (igrad = 0; igrad<xform_grad.size(); igrad++)
    grad_jones[igrad] = transform (xform_grad[igrad], input_jones);

  // map the transformation parameter gradient elements
  ProjectGradient (transformation, grad_jones, *grad);

  // compute the partial derivatives wrt input[current_source] parameters
  for (igrad=0; igrad<input_grad.size(); igrad++)
    input_grad[igrad] = transform (xform, input_grad[igrad]);

  // map the input parameter gradient elements
  ProjectGradient (input, input_grad, *grad);

  if (verbose) {
    cerr << "MEAL::MuellerTransformation::evaluate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << " " << get_param_name(i)
	   << "=" << (*grad)[i] << endl;
  }
}
