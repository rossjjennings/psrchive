#include "MEAL/CongruenceTransformation.h"
#include "MEAL/ProjectGradient.h"

using namespace std;

MEAL::CongruenceTransformation::CongruenceTransformation ()
{
}

MEAL::CongruenceTransformation::~CongruenceTransformation ()
{
}

/*! This method unmaps the old transformation before mapping xform */
void 
MEAL::CongruenceTransformation::set_transformation (Complex2* xform)
{
  if (!xform)
    return;

  if (transformation) {
    if (verbose)
      cerr << "MEAL::CongruenceTransformation::set_transformation"
	" unmap old transformation" << endl;
    unmap (transformation, false);
  }

  transformation = xform;

  if (verbose)
    cerr << "MEAL::CongruenceTransformation::set_transformation"
      " map new transformation" << endl;

  map (transformation);
}

//! Get the transformation, \f$ J \f$
MEAL::Complex2* 
MEAL::CongruenceTransformation::get_transformation ()
{
  return transformation;
}


/*! This method unmaps the old input before mapping xform */
void 
MEAL::CongruenceTransformation::set_input (Complex2* xform)
{
  if (!xform)
    return;

  if (input) {
    if (verbose)
      cerr << "MEAL::CongruenceTransformation::set_input"
	" unmap old input" << endl;
    unmap (input, false);
  }

  input = xform;

  if (verbose)
    cerr << "MEAL::CongruenceTransformation::set_input"
      " map new input" << endl;

  map (input);
}

//! Get the input, \f$ \rho \f$
MEAL::Complex2* 
MEAL::CongruenceTransformation::get_input ()
{
  return input;
}


//! Returns \f$ \rho^\prime_j \f$ and its gradient
void 
MEAL::CongruenceTransformation::calculate (Jones<double>& result,
						  std::vector<Jones<double> >* grad)
{
  if (verbose)
    cerr << "MEAL::CongruenceTransformation::calculate" << endl;

  // gradient of transformation
  std::vector<Jones<double> > xform_grad;
  std::vector<Jones<double> > *xform_grad_ptr = 0;

  // gradient of input
  std::vector<Jones<double> > input_grad;
  std::vector<Jones<double> > *input_grad_ptr = 0;

  if (grad) {

    xform_grad_ptr = &xform_grad;
    input_grad_ptr = &input_grad;

  }

  // compute xform and partial derivatives with respect to xform parameters
  Jones<double> xform_jones = transformation->evaluate (xform_grad_ptr);
  Jones<double> xform_herm = herm (xform_jones);

  // compute input and partial derivatives with respect to input parameters
  Jones<double> input_jones = input->evaluate (input_grad_ptr);

  // set the result
  result = xform_jones * input_jones * xform_herm;

  if (verbose)
    cerr << "MEAL::CongruenceTransformation::evaluate result\n"
	 "   " << result << endl;

  if (!grad)
    return;

  // compute the partial derivatives

  unsigned igrad;

  // resize the gradient for the partial derivatives wrt all parameters
  grad->resize (get_nparam());
  for (igrad = 0; igrad<grad->size(); igrad++)
    (*grad)[igrad] = 0;

  // compute the partial derivatives wrt transformation parameters
  for (igrad = 0; igrad<xform_grad.size(); igrad++)
    xform_grad[igrad] = xform_jones * input_jones * herm(xform_grad[igrad])
      + xform_grad[igrad] * input_jones * xform_herm;  

  // map the transformation parameter gradient elements
  ProjectGradient (transformation, xform_grad, *grad);

  // compute the partial derivatives wrt input[current_source] parameters
  for (igrad=0; igrad<input_grad.size(); igrad++)
    input_grad[igrad] = xform_jones * input_grad[igrad] * xform_herm;

  // map the input parameter gradient elements
  ProjectGradient (input, input_grad, *grad);

  if (verbose) {
    cerr << "MEAL::CongruenceTransformation::evaluate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << " " << get_param_name(i)
	   << "=" << (*grad)[i] << endl;
  }
}
