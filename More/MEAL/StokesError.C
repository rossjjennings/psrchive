/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/StokesError.h"

#include "Pauli.h"
#include "complex_public.h"

//! Default constructor
MEAL::StokesError::StokesError ()
{
  Jones<MEAL::ScalarMath> J;

  for (unsigned i=0; i<2; i++)
    for (unsigned j=0; j<2; j++) {
      pub(J(i,j)).real = pub(xform(i,j)).real;
      pub(J(i,j)).imag = pub(xform(i,j)).imag;
    }

  input = Vector< 4, Estimate<double> > ();

  output = transform (input, J);

  for (unsigned i=0; i<4; i++)
    if (output[i].get_expression()->get_nparam() != 4)
      throw Error (InvalidState, "MEAL::StokesError ctor",
		   "output Stokes parameters have more than 4 dof");
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

//! Set the transformation from template to observation
void MEAL::StokesError::set_transformation (const Jones<double>& J)
{
  for (unsigned i=0; i<2; i++)
    for (unsigned j=0; j<2; j++) {
      pub(xform(i,j)).real.set_value( pub(J(i,j)).real );
      pub(xform(i,j)).imag.set_value( pub(J(i,j)).imag );
    }
}

//! Get the variances of the output Stokes parameters
void MEAL::StokesError::get_variance (Stokes<double>& var)
{
  for (unsigned ipol=0; ipol< 4; ipol++)
    var[ipol] = output[ipol].get_Estimate().get_variance();
}
