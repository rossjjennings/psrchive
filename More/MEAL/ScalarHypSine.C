#include "MEPL/ScalarHypSine.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarHypSine::get_name () const
{
  return "ScalarHypSine";
}

//! The function
double Model::ScalarHypSine::function (double x) const
{
  return sinh(x); 
}

//! And its first derivative
double Model::ScalarHypSine::dfdx (double x) const
{ 
  return cosh(x);
}
