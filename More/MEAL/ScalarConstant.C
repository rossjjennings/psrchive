#include "MEAL/ScalarConstant.h"

using namespace std;

//! Default constructor
MEAL::ScalarConstant::ScalarConstant (double _value)
{
  value = _value;
  set_evaluation_changed (false);
}

//! Copy constructor
MEAL::ScalarConstant::ScalarConstant (const ScalarConstant& scalar)
{
  value = scalar.value;
  set_evaluation_changed (false);
}

//! Assignment operator
const MEAL::ScalarConstant&
MEAL::ScalarConstant::operator = (const ScalarConstant& scalar)
{
  value = scalar.value;
  return *this;
}

//! Destructor
MEAL::ScalarConstant::~ScalarConstant ()
{
}

//! Return the name of the class
string MEAL::ScalarConstant::get_name () const
{
  return "ScalarConstant";
}

double MEAL::ScalarConstant::evaluate (std::vector<double >* grad) const
{
  if (grad)
    grad->resize(0);

  return value;
}
