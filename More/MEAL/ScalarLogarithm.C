#include "MEPL/ScalarLogarithm.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarLogarithm::get_name () const
{
  return "ScalarLogarithm";
}

//! The function
double Model::ScalarLogarithm::function (double x) const
{
  if (x < 0.0)
    throw Error (InvalidState, "Model::ScalarLogarithm::function",
		 "x < 0");
  return log(x); 
}

//! And its first derivative
double Model::ScalarLogarithm::dfdx (double x) const
{ 
  if (x < 0.0)
    throw Error (InvalidState, "Model::ScalarLogarithm::dfdx",
		 "x < 0");
  return 1/x;
}
