/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/StokesError.h"
#include "Pauli.h"

#include "complex_public.h"

void copy (Jones<MEAL::ScalarMath>& to, Jones<MEAL::ScalarValue>& from)
{
  for (unsigned i=0; i<2; i++) {
    for (unsigned j=0; j<2; j++) {
      pub(to(i,j)).real = pub(from(i,j)).real;
      pub(to(i,j)).imag = pub(from(i,j)).imag;
    }
  }
}

//! Default constructor
MEAL::StokesError::StokesError ()
{
  Jones<MEAL::ScalarMath> J;
  copy (J, xform);

  input = Vector< 4, Estimate<double> > ();

  output = transform (input, J);

  for (unsigned i=0; i<4; i++)
    if (output[i].get_expression()->get_nparam() != 4)
      throw Error (InvalidState, "MEAL::StokesError ctor",
		   "output Stokes parameter has more than 4 dof");

  gradient_built = false;
}

void MEAL::StokesError::build_gradient ()
{
  if (gradient_built)
    return;

  Jones<MEAL::ScalarMath> J;
  copy (J, xform);

  Jones<MEAL::ScalarMath> J_grad;
  copy (J_grad, xform_grad);

  Jones<MEAL::ScalarMath> rho = convert(input);

  output_grad = coherency ( J_grad * rho * herm(J) + J * rho * herm(J_grad) );

  for (unsigned i=0; i<4; i++)
    if (output_grad[i].get_expression()->get_nparam() != 4)
      throw Error (InvalidState, "MEAL::StokesError ctor",
		   "output Stokes parameters gradient has more than 4 dof");
}

//! Set the variances of the input Stokes parameters
void MEAL::StokesError::set_variance (const Stokes<double>& var)
{
  Estimate<double> v;

  for (unsigned ipol=0; ipol < 4; ipol++) {
    v.var = var[ipol];
    input[ipol].get_expression()->set_Estimate( 0, v );
  }
}


void copy (Jones<MEAL::ScalarValue>& to, const Jones<double>& from)
{
  for (unsigned i=0; i<2; i++) {
    for (unsigned j=0; j<2; j++) {
      pub(to(i,j)).real.set_value( pub(from(i,j)).real );
      pub(to(i,j)).imag.set_value( pub(from(i,j)).imag );
    }
  }
}

//! Set the transformation from template to observation
void MEAL::StokesError::set_transformation (const Jones<double>& J)
{
  copy (xform, J);
}

//! Get the variances of the output Stokes parameters
void MEAL::StokesError::get_variance (Stokes<double>& var)
{
  for (unsigned ipol=0; ipol< 4; ipol++)
    var[ipol] = output[ipol].get_Estimate().get_variance();
}

//! Set the transformation gradient component
void MEAL::StokesError::set_transformation_gradient (const Jones<double>& grad)
{
  copy (xform_grad, grad);
}

//! Get the variances of the output Stokes parameters gradient component
void MEAL::StokesError::get_variance_gradient (Stokes<double>& var_grad)
{
  build_gradient ();
  for (unsigned ipol=0; ipol< 4; ipol++)
    var_grad[ipol] = output_grad[ipol].get_Estimate().get_variance();
}
