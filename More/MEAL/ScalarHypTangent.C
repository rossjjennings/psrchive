#include "MEPL/ScalarHypTangent.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarHypTangent::get_name () const
{
  return "ScalarHypTangent";
}

//! The function
double Model::ScalarHypTangent::function (double x) const
{
  return tanh(x); 
}

//! And its first derivative
double Model::ScalarHypTangent::dfdx (double x) const
{ 
  double coshx = cosh(x);
  return 1.0/(coshx*coshx);
}
