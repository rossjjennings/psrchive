#include "MEPL/ScalarSine.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarSine::get_name () const
{
  return "ScalarSine";
}

//! The function
double Model::ScalarSine::function (double x) const
{
  return sin(x); 
}

//! And its first derivative
double Model::ScalarSine::dfdx (double x) const
{ 
  return cos(x);
}
