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
  beta = exp( *log_beta ).get_expression();
  boost->set_beta (beta);

  log_intensity = new ScalarParameter;
  log_intensity->set_value_name( "log(I)" );
  intensity = exp( *log_intensity ).get_expression();
  Complex2* gain = cast<Complex2> (intensity.get());

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

//! Get the model of the unit vector
MEAL::UnitTangent* MEAL::ModeCoherency::get_axis ()
{
  return axis;
}

//! Set the model of the unit vector
void MEAL::ModeCoherency::set_axis (UnitTangent* _axis)
{
  axis = _axis;
  boost->set_axis (axis);
}

//! Get beta
MEAL::Scalar* MEAL::ModeCoherency::get_beta ()
{
  return beta;
}

//! Get the intensity
MEAL::Scalar* MEAL::ModeCoherency::get_intensity ()
{
  return intensity;
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

  log_intensity->set_value ( ::log( 0.5 * stokes[0] * ::sqrt(1.0-p*p) ) );
  log_beta->set_value ( ::log(::atanh(p)) );
  axis->set_vector (stokes.get_vector());

  cerr << "MEAL::ModeCoherency::set_stokes in=" << stokes
       << " result=" << get_stokes() << endl;
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

  log_intensity->set_value ( log( 0.5 * stokes[0] * sqrt(1-p*p) ) );
  log_beta->set_value ( log(atanh(p)) );
  axis->set_vector (stokes.get_vector());
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > MEAL::ModeCoherency::get_stokes () const
{
  return coherency( estimate() );
}
