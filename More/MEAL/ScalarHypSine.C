#include "MEAL/ScalarHypSine.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarHypSine::get_name () const
{
  return "ScalarHypSine";
}

//! The function
double MEAL::ScalarHypSine::function (double x) const
{
  return sinh(x); 
}

//! And its first derivative
double MEAL::ScalarHypSine::dfdx (double x) const
{ 
  return cosh(x);
}
