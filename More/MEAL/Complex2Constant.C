#include "Complex2Constant.h"

//! Default constructor
Calibration::Complex2Constant::Complex2Constant (const Jones<double>& _jones)
{
  jones = _jones;
  set_evaluation_changed (false);
}

//! Copy constructor
Calibration::Complex2Constant::Complex2Constant (const Complex2Constant& other)
{
  jones = other.jones;
  set_evaluation_changed (false);
}

//! Assignment operator
const Calibration::Complex2Constant&
Calibration::Complex2Constant::operator = (const Complex2Constant& other)
{
  jones = other.jones;
  return *this;
}

//! Destructor
Calibration::Complex2Constant::~Complex2Constant ()
{
}

//! Return the name of the class
string Calibration::Complex2Constant::get_name () const
{
  return "Complex2Constant";
}


Jones<double>
Calibration::Complex2Constant::evaluate (vector<Jones<double> >* grad)
  const
{
  if (grad)
    grad->resize(0);

  return jones;
}
