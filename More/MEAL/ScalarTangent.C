#include "MEPL/ScalarTangent.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarTangent::get_name () const
{
  return "ScalarTangent";
}

//! The function
double Model::ScalarTangent::function (double x) const
{
  return tan(x); 
}

//! And its first derivative
double Model::ScalarTangent::dfdx (double x) const
{
  double cosx = cos(x);
  return 1.0/(cosx*cosx);
}
