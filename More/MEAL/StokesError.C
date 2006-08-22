/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/StokesError.h"
#include "Pauli.h"

#include <iostream>

template<class T>
class public_complex {
public:
  T real;
  T imag;
};

template<class T>
public_complex<T>* pub (std::complex<T>& c)
{
  return reinterpret_cast<public_complex<T>*>( &c );
}

template<class T>
const public_complex<T>* pub (const std::complex<T>& c)
{
  return reinterpret_cast<const public_complex<T>*>( &c );
}

//! Default constructor
MEAL::StokesError::StokesError ()
{
  Jones<MEAL::ScalarMath> J;

  for (unsigned i=0; i<2; i++)
    for (unsigned j=0; j<2; j++) {
      pub(J(i,j))->real = pub(xform(i,j))->real;
      pub(J(i,j))->imag = pub(xform(i,j))->imag;
    }

  output = transform (input, J);
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
      pub(xform(i,j))->real.set_value( pub(J(i,j))->real );
      pub(xform(i,j))->imag.set_value( pub(J(i,j))->imag );
    }
}

//! Get the variances of the output Stokes parameters
void MEAL::StokesError::get_variance (Stokes<double>& var)
{
  for (unsigned ipol=0; ipol< 4; ipol++)
    var[ipol] = output[ipol].get_Estimate().get_variance();
}
