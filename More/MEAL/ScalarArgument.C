#include "MEPL/ScalarArgument.h"

//! Default constructor
Model::ScalarArgument::ScalarArgument ()
{
}

//! Copy constructor
Model::ScalarArgument::ScalarArgument (const ScalarArgument& scalar)
  : Univariate (scalar)
{
}

//! Assignment operator
const Model::ScalarArgument&
Model::ScalarArgument::operator = (const ScalarArgument& scalar)
{
  Univariate::operator = (scalar);
  return *this;
}

//! Destructor
Model::ScalarArgument::~ScalarArgument ()
{
}

//! Return the name of the class
string Model::ScalarArgument::get_name () const
{
  return "ScalarArgument";
}

double Model::ScalarArgument::evaluate (vector<double >* grad) const
{
  if (grad)
    grad->resize(0);

  return abscissa;
}

