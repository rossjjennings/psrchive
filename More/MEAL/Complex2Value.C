#include "MEPL/Complex2Value.h"

Model::Complex2Value::Complex2Value (const Jones<double>& _value)
  : OptimizedComplex2 (0)
{
  value = _value;
}

//! Return the name of the class
string Model::Complex2Value::get_name () const
{
  return "Complex2Value";
}

void Model::Complex2Value::set_value (const Jones<double>& _value)
{
  value = _value;
  set_evaluation_changed();
}

Jones<double> Model::Complex2Value::get_value () const
{
  return value;
}

//! Return the value (and gradient, if requested) of the function
void Model::Complex2Value::calculate (Jones<double>& result, 
					    vector< Jones<double> >* grad)
{
  result = value;

  if (grad)
    grad->resize (0);
}
