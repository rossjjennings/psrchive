/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ModeCoherency.h"
#include "MEAL/BoostUnion.h"
#include "MEAL/UnitTangent.h"

#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ProductRule.h"
#include "MEAL/Cast.h"

#include "Pauli.h"
#include "Error.h"

#include <cmath>

using namespace std;

void MEAL::ModeCoherency::init ()
{
  boost = new BoostUnion;

  axis = new UnitTangent;
  boost->set_axis (axis);

  log_beta = new ScalarParameter;
  log_beta->set_value_name( "log(beta)" );
  boost->set_beta (exp( *log_beta ).get_expression());

  log_intensity = new ScalarParameter;
  log_intensity->set_value_name( "log(I)" );

  Complex2* gain = cast<Complex2> (exp( *log_intensity ).get_expression());

  ProductRule<Complex2>* product = new ProductRule<Complex2>;
  product->add_model( gain );
  product->add_model( boost );

  wrap (product);
}

MEAL::ModeCoherency::ModeCoherency ()
{
  init ();
}

//! Copy constructor
MEAL::ModeCoherency::ModeCoherency (const ModeCoherency& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::ModeCoherency&
MEAL::ModeCoherency::operator = (const ModeCoherency& that)
{
  if (this == &that)
    return *this;

  axis->copy( that.axis );
  log_beta->copy( that.log_beta );
  log_intensity->copy( that.log_intensity );

  return *this;
}

//! Destructor
MEAL::ModeCoherency::~ModeCoherency ()
{
}

//! Return the name of the class
string MEAL::ModeCoherency::get_name () const
{
  return "ModeCoherency";
}

//! Set the Stokes parameters of the model
void MEAL::ModeCoherency::set_stokes (const Stokes<double>& stokes)
{
  if (stokes[0] <= 0.0)
    throw Error (InvalidParam, "MEAL::ModeCoherency::set_stokes",
		 "total intensity I=%lf <= 0", stokes[0]);

  double p = stokes.abs_vect() / stokes[0];

  if (p >= 1)
    throw Error (InvalidParam, "MEAL::ModeCoherency::set_stokes",
		 "degree of polarization p=%lf >= 1", p);

  if (p == 0.0)
    throw Error (InvalidParam, "MEAL::ModeCoherency::set_stokes",
		 "degree of polarization p==0");

  log_intensity->set_value ( ::log(stokes[0]) );
  log_beta->set_value ( ::log(::atanh(p)) );
  axis->set_vector (stokes.get_vector());
}

//! Set the Stokes parameters of the model
void MEAL::ModeCoherency::set_stokes (const Stokes<Estimate<double> >& stokes)
{
  if (stokes[0].get_value() <= 0.0)
    throw Error (InvalidParam, "MEAL::ModeCoherency::set_stokes",
		 "total intensity I=%lf <= 0", stokes[0].get_value());

  Estimate<double> p = stokes.abs_vect() / stokes[0];

  if (p.get_value() >= 1)
    throw Error (InvalidParam, "MEAL::ModeCoherency::set_stokes",
		 "degree of polarization p=%lf >= 1", p.get_value());

  if (p.get_value() == 0.0)
    throw Error (InvalidParam, "MEAL::ModeCoherency::set_stokes",
		 "degree of polarization p==0");

  log_intensity->set_value ( log(stokes[0]) );
  log_beta->set_value ( log(atanh(p)) );
  axis->set_vector (stokes.get_vector());
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > MEAL::ModeCoherency::get_stokes () const
{
  return coherency( estimate() );
}
