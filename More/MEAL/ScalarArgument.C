#include "ScalarArgument.h"

//! Default constructor
Calibration::ScalarArgument::ScalarArgument ()
{
}

//! Copy constructor
Calibration::ScalarArgument::ScalarArgument (const ScalarArgument& scalar)
  : Univariate (scalar)
{
}

//! Assignment operator
const Calibration::ScalarArgument&
Calibration::ScalarArgument::operator = (const ScalarArgument& scalar)
{
  Univariate::operator = (scalar);
  return *this;
}

//! Destructor
Calibration::ScalarArgument::~ScalarArgument ()
{
}

//! Return the name of the class
string Calibration::ScalarArgument::get_name () const
{
  return "ScalarArgument";
}

double Calibration::ScalarArgument::evaluate (vector<double >* grad) const
{
  if (grad)
    grad->resize(0);

  return abscissa;
}

