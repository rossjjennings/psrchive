#include "ScalarConstant.h"

//! Default constructor
Calibration::ScalarConstant::ScalarConstant (double _value)
{
  value = _value;
  set_evaluation_changed (false);
}

//! Copy constructor
Calibration::ScalarConstant::ScalarConstant (const ScalarConstant& scalar)
{
  value = scalar.value;
  set_evaluation_changed (false);
}

//! Assignment operator
const Calibration::ScalarConstant&
Calibration::ScalarConstant::operator = (const ScalarConstant& scalar)
{
  value = scalar.value;
  return *this;
}

//! Destructor
Calibration::ScalarConstant::~ScalarConstant ()
{
}

//! Return the name of the class
string Calibration::ScalarConstant::get_name () const
{
  return "ScalarConstant";
}

double Calibration::ScalarConstant::evaluate (vector<double >* grad) const
{
  if (grad)
    grad->resize(0);

  return value;
}
