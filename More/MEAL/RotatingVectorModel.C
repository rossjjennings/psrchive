/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"

using namespace std;

void MEAL::RotatingVectorModel::init ()
{
  if (verbose)
    cerr << "MEAL::RotatingVectorModel::init" << endl;

  /*
    SumRule is used to optionally add alpha to zeta, turning this
    parameter into beta (see use_impact method).
  */
  
  line_of_sight = new ScalarParameter;
  line_of_sight->set_value_name ("zeta");

  zeta_sum = new SumRule<Scalar>;
  zeta_sum->add_model( line_of_sight );

  magnetic_axis = new ScalarParameter;
  magnetic_axis->set_value_name ("alpha");

  ScalarMath lon = *longitude - *magnetic_meridian;

  /*
    The original RVM sign convention for PA is opposite to that of the IAU.
    See Everett & Weisberg (2001; ApJ 553:341) for more details.

    This is achieved here by reversing the sign of the denominator, x.
  */

  ScalarMath y = sin(*magnetic_axis) * sin(lon);

  ScalarMath x = sin(*magnetic_axis) * cos(*zeta_sum) * cos(lon)
    - cos(*magnetic_axis) * sin(*zeta_sum);

  set_atan_Psi (y, x);
}

//! Switch to using impact as a free parameter
void MEAL::RotatingVectorModel::use_impact (bool flag)
{
  if (flag)
  {
    if (impact)
      return;

    Estimate<double> beta 
      = line_of_sight->get_value() - magnetic_axis->get_value();

    impact = line_of_sight;  // transfer line_of_sight to impact
    impact->set_value( beta );
    impact->set_value_name( "beta" );

    line_of_sight = 0; // pointer set to zero

    zeta_sum->add_model( magnetic_axis );
  }
  else
  {
    if (!impact)
      return;

    Estimate<double> zeta 
      = impact->get_value() + magnetic_axis->get_value();

    line_of_sight = impact;
    line_of_sight->set_value( zeta );
    line_of_sight->set_value_name( "zeta" );

    impact = 0;

    zeta_sum->remove_model( magnetic_axis );
  }  
}

MEAL::RotatingVectorModel::RotatingVectorModel ()
{
  init ();
}

//! Copy constructor
MEAL::RotatingVectorModel::RotatingVectorModel (const RotatingVectorModel& rvm)
{
  init ();
  operator = (rvm);
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

//! Return the name of the class
string MEAL::RotatingVectorModel::get_name () const
{
  return "RotatingVectorModel";
}

