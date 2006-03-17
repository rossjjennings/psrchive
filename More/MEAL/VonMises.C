/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/VonMises.h"
#include "MEAL/InverseRule.h"
#include "MEAL/ScalarBesselI0.h"
#include "MEAL/ScalarConstant.h"
 
using namespace std;

//InverseRule needs this... maybe not the best place to define it??
static inline double inv(double x) { return 1.0/x;} 

MEAL::VonMises::VonMises () 
  : x(*new ScalarArgument)
{
  centre.set_value_name("centre");
  concentration.set_value_name("concentration");
  
  //  x.changed.connect(this, &MEAL::VonMises::attribute_changed);

  ScalarMath numerator = exp (ScalarMath(concentration)
			      *cos(ScalarMath(x)-ScalarMath(centre)));
  ScalarBesselI0 *bessel = new ScalarBesselI0;
  bessel->set_model(&concentration);
  ScalarMath denominator(*bessel);
  
  InverseRule<Scalar> *inverse_denominator = new InverseRule<Scalar>;
  inverse_denominator->set_model(denominator.get_expression());
  
  *this *= numerator.get_expression();
  *this *= inverse_denominator;
  *this *= new ScalarConstant (0.5/M_PI);
}



//! Set the centre
void MEAL::VonMises::set_centre (double centre_)
{
  centre.set_param(0, centre_);
}

//! Get the centre
double MEAL::VonMises::get_centre () const
{
  return centre.get_param (0);
}

//! Set the concentration
void MEAL::VonMises::set_concentration (double concentration_)
{
  concentration.set_param (0, concentration_);
}

//! Get the concentration
double MEAL::VonMises::get_concentration () const
{
  return concentration.get_param (0);
}

#if 0
void MEAL::VonMises::set_argument (unsigned dimension,
	 			   Argument* argument)
 
{
  x.set_argument(dimension, argument);
}
#endif

std::string 
MEAL::VonMises::get_name() const
{
  return "VonMises";
}

void MEAL::VonMises::parse (const string& line)
{
  Function::parse(line); // avoid using inherited GroupRule::parse()
}

