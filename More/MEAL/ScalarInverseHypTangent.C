#include "MEAL/ScalarInverseHypTangent.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarInverseHypTangent::get_name () const
{
  return "ScalarInverseHypTangent";
}

//! The function
double MEAL::ScalarInverseHypTangent::function (double x) const
{
  return atanh(x); 
}

//! And its first derivative
double MEAL::ScalarInverseHypTangent::dfdx (double x) const
{ 
  return 1.0/(1.0 - x*x);
}
