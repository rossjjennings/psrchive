#include "MEPL/ScalarConstant.h"

//! Default constructor
Model::ScalarConstant::ScalarConstant (double _value)
{
  value = _value;
  set_evaluation_changed (false);
}

//! Copy constructor
Model::ScalarConstant::ScalarConstant (const ScalarConstant& scalar)
{
  value = scalar.value;
  set_evaluation_changed (false);
}

//! Assignment operator
const Model::ScalarConstant&
Model::ScalarConstant::operator = (const ScalarConstant& scalar)
{
  value = scalar.value;
  return *this;
}

//! Destructor
Model::ScalarConstant::~ScalarConstant ()
{
}

//! Return the name of the class
string Model::ScalarConstant::get_name () const
{
  return "ScalarConstant";
}

double Model::ScalarConstant::evaluate (vector<double >* grad) const
{
  if (grad)
    grad->resize(0);

  return value;
}
