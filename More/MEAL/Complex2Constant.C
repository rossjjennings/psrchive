#include "MEPL/Complex2Constant.h"

//! Default constructor
Model::Complex2Constant::Complex2Constant (const Jones<double>& _jones)
{
  jones = _jones;
  set_evaluation_changed (false);
}

//! Copy constructor
Model::Complex2Constant::Complex2Constant (const Complex2Constant& other)
{
  jones = other.jones;
  set_evaluation_changed (false);
}

//! Assignment operator
const Model::Complex2Constant&
Model::Complex2Constant::operator = (const Complex2Constant& other)
{
  jones = other.jones;
  return *this;
}

//! Destructor
Model::Complex2Constant::~Complex2Constant ()
{
}

//! Return the name of the class
string Model::Complex2Constant::get_name () const
{
  return "Complex2Constant";
}


Jones<double>
Model::Complex2Constant::evaluate (vector<Jones<double> >* grad)
  const
{
  if (grad)
    grad->resize(0);

  return jones;
}
