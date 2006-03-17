/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"

using namespace std;

void MEAL::RotatingVectorModel::init ()
{
  ScalarArgument* argument = new ScalarArgument; 

  reference_position_angle = new ScalarParameter;
  line_of_sight = new ScalarParameter;
  magnetic_axis = new ScalarParameter;
  magnetic_meridian = new ScalarParameter;

  ScalarMath longitude = *argument - *magnetic_meridian;

  ScalarMath numerator = sin(*magnetic_axis) * sin(longitude);
  ScalarMath denominator = cos(*magnetic_axis) * sin(*line_of_sight)
    - sin(*magnetic_axis) * cos(*line_of_sight) * cos(longitude);  

  ScalarMath result = atan2(numerator,denominator) + *reference_position_angle;

  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);
  copy_univariate_policy (argument);
}

MEAL::RotatingVectorModel::RotatingVectorModel ()
{
  init ();
}

//! Copy constructor
MEAL::RotatingVectorModel::RotatingVectorModel (const RotatingVectorModel& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::RotatingVectorModel&
MEAL::RotatingVectorModel::operator = (const RotatingVectorModel& copy)
{
  Univariate<Scalar>::operator = (copy);
  return *this;
}

MEAL::RotatingVectorModel::~RotatingVectorModel ()
{
}

void MEAL::RotatingVectorModel::set_reference_position_angle
(const Estimate<double>& position_angle)
{
  reference_position_angle->set_value (position_angle);
}

Estimate<double> 
MEAL::RotatingVectorModel::get_reference_position_angle () const
{
  return reference_position_angle->get_value ();
}

//! Set the latitude of the line of sight
void MEAL::RotatingVectorModel::set_line_of_sight (const Estimate<double>& l)
{
  line_of_sight->set_value (l);
}

//! Get the latitude of the line of sight
Estimate<double> MEAL::RotatingVectorModel::get_line_of_sight () const
{
  return line_of_sight->get_value ();
}

//! Set the latitude of the magnetic axis
void MEAL::RotatingVectorModel::set_magnetic_axis (const Estimate<double>& m)
{
  magnetic_axis->set_value (m);
}

//! Get the latitude of the magnetic axis
Estimate<double> MEAL::RotatingVectorModel::get_magnetic_axis () const
{
  return magnetic_axis->get_value ();
}

//! Set the longitude of the magnetic meridian
void 
MEAL::RotatingVectorModel::set_magnetic_meridian (const Estimate<double>& m)
{
  magnetic_meridian->set_value (m);
}

//! Get the longitude of the magnetic meridian
Estimate<double> MEAL::RotatingVectorModel::get_magnetic_meridian () const
{
  return magnetic_meridian->get_value ();
}


//! Return the name of the class
string MEAL::RotatingVectorModel::get_name () const
{
  return "RotatingVectorModel";
}

