#include "MEPL/ScalarCosine.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarCosine::get_name () const
{
  return "ScalarCosine";
}

//! The function
double Model::ScalarCosine::function (double x) const
{
  return cos(x); 
}

//! And its first derivative
double Model::ScalarCosine::dfdx (double x) const
{ 
  return -sin(x);
}
