#include "MEPL/ScalarAbsolute.h"
#include <math.h>

//! Return the name of the class
string Model::ScalarAbsolute::get_name () const
{
  return "ScalarAbsolute";
}

//! The function
double Model::ScalarAbsolute::function (double x) const
{
  return fabs(x); 
}

//! And its first derivative
double Model::ScalarAbsolute::dfdx (double x) const
{ 
  if (x < 0)
    return -1;
  else
    return 1;
}
