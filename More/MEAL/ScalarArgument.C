#include "MEAL/ScalarArgument.h"

using namespace std;

//! Default constructor
MEAL::ScalarArgument::ScalarArgument ()
{
}

//! Copy constructor
MEAL::ScalarArgument::ScalarArgument (const ScalarArgument& scalar)
  : Univariate (scalar)
{
}

//! Assignment operator
const MEAL::ScalarArgument&
MEAL::ScalarArgument::operator = (const ScalarArgument& scalar)
{
  Univariate::operator = (scalar);
  return *this;
}

//! Destructor
MEAL::ScalarArgument::~ScalarArgument ()
{
}

//! Return the name of the class
string MEAL::ScalarArgument::get_name () const
{
  return "ScalarArgument";
}

double MEAL::ScalarArgument::evaluate (std::vector<double >* grad) const
{
  if (grad)
    grad->resize(0);

  return abscissa;
}

