#include "MEPL/ScalarInverseHypTangent.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarInverseHypTangent::get_name () const
{
  return "ScalarInverseHypTangent";
}

//! The function
double Model::ScalarInverseHypTangent::function (double x) const
{
  return atanh(x); 
}

//! And its first derivative
double Model::ScalarInverseHypTangent::dfdx (double x) const
{ 
  return 1.0/(1.0 - x*x);
}
