#include "MEPL/ScalarHypCosine.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarHypCosine::get_name () const
{
  return "ScalarHypCosine";
}

//! The function
double Model::ScalarHypCosine::function (double x) const
{
  return cosh(x); 
}

//! And its first derivative
double Model::ScalarHypCosine::dfdx (double x) const
{ 
  return sinh(x);
}
