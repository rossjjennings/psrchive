#include "MEAL/Complex2Constant.h"

//! Default constructor
MEAL::Complex2Constant::Complex2Constant (const Jones<double>& _jones)
{
  jones = _jones;
  set_evaluation_changed (false);
}

//! Copy constructor
MEAL::Complex2Constant::Complex2Constant (const Complex2Constant& other)
{
  jones = other.jones;
  set_evaluation_changed (false);
}

//! Assignment operator
const MEAL::Complex2Constant&
MEAL::Complex2Constant::operator = (const Complex2Constant& other)
{
  jones = other.jones;
  return *this;
}

//! Destructor
MEAL::Complex2Constant::~Complex2Constant ()
{
}

//! Return the name of the class
std::string MEAL::Complex2Constant::get_name () const
{
  return "Complex2Constant";
}


Jones<double>
MEAL::Complex2Constant::evaluate (std::vector<Jones<double> >* grad)
  const
{
  if (grad)
    grad->resize(0);

  return jones;
}
