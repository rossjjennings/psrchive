#include "MEPL/ScalarRoot.h"
#include <math.h>

//
//
//
string Model::ScalarRoot::get_name () const
{
  return "ScalarRoot";
}

//
//
//
double Model::ScalarRoot::function (double x) const
{
  return sqrt(x); 
}

//
//
//
double Model::ScalarRoot::dfdx (double x) const
{ 
  return 0.5/sqrt(x);
}
