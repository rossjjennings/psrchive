/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MEAL/Polynomial2D.h"

using namespace std;
 
void MEAL::Polynomial2D::init()
{
  chain.set_model ( &poly );

  copy_parameter_policy  (&chain);
  copy_evaluation_policy (&chain);
}

MEAL::Polynomial2D::Polynomial2D (unsigned ncoeff, unsigned mcoeff)
: Multivariate<Scalar> (2)
{
  init ();

  if (ncoeff == 0 || mcoeff == 0)
    return;

  resize (ncoeff, mcoeff);
}

void MEAL::Polynomial2D::resize (unsigned ncoeff, unsigned mcoeff)
{
  ncoef.first = ncoeff;
  ncoef.second = mcoeff;

  poly.resize (ncoeff);

  if (ncoeff == 0)
    return;

  // poly.set_infit (0, false);

  coefficients.resize (ncoeff - 1);
  for (unsigned i=0; i < coefficients.size(); i++)
  {
    coefficients[i].resize (mcoeff);

    for (unsigned j=0; j<mcoeff; j++)
      coefficients[i].set_param_name (j, "c_" + tostring(i+1) + "_" + tostring(j));

    chain.set_constraint (i+1, &coefficients[i]);
  }
}

//! Set the abscissa value
void MEAL::Polynomial2D::set_abscissa_value (unsigned dim, double value)
{
  Multivariate<Scalar>::set_abscissa_value (dim, value);

  if (dim == 0)
    poly.set_abscissa (value);
  else if (dim == 1)
    for (unsigned i=0; i < coefficients.size(); i++)
      coefficients[i].set_abscissa (value);
  else
    throw Error (InvalidParam, "Polynomial2D::set_abscissa",
                 "invalid dim=%u > 1", dim);
}

//! Set the abscissa offset
void MEAL::Polynomial2D::set_abscissa_offset (unsigned dim, double offset)
{
  if (dim == 0)
    poly.set_abscissa_offset (offset);
  else if (dim == 1)
    for (unsigned i=0; i < coefficients.size(); i++)
      coefficients[i].set_abscissa_offset (offset);
  else
    throw Error (InvalidParam, "Polynomial2D::set_abscissa",
                 "invalid dim=%u > 1", dim);
}

//! Copy constructor
MEAL::Polynomial2D::Polynomial2D (const Polynomial2D& copy)
: Multivariate<Scalar> (2)
{
  init ();
  operator = (copy);
}

//! Assignment operator
const MEAL::Polynomial2D&
MEAL::Polynomial2D::operator = (const Polynomial2D& copy)
{
  if (this == &copy)
    return *this;

  set_ncoef( copy.get_ncoef() );

  poly = copy.poly;
  for (unsigned i=0; i < coefficients.size(); i++)
    coefficients[i] = copy.coefficients[i];

  Multivariate<Scalar>::operator = (copy);
  return *this;
}
  
std::string MEAL::Polynomial2D::get_name() const
{
  return "Polynomial2D";
}

#include "MEAL/FunctionInterface.h"
#include "pairutil.h"

class MEAL::Polynomial2D::Interface : public Function::Interface<MEAL::Polynomial2D>
{
  public:
    //! Default constructor that takes an optional instance
    Interface ( Polynomial2D* = 0 );
};

MEAL::Polynomial2D::Interface::Interface ( Polynomial2D* s_instance )
: Function::Interface<MEAL::Polynomial2D> ( s_instance )
{
  add( &Polynomial2D::get_ncoef,
       &Polynomial2D::set_ncoef,
       "ncoef", "Number of coefficients in each dimension" );
}

//! Return a text interface that can be used to access this instance
TextInterface::Parser* MEAL::Polynomial2D::get_interface ()
{
  return new Interface (this);
}

