#include "MEPL/ScalarExponential.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarExponential::get_name () const
{
  return "ScalarExponential";
}

//! The function
double Model::ScalarExponential::function (double x) const
{
  return exp(x); 
}

//! And its first derivative
double Model::ScalarExponential::dfdx (double x) const
{ 
  return exp(x);
}
