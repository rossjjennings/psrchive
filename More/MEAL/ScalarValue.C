#include "MEPL/ScalarValue.h"

Model::ScalarValue::ScalarValue (Estimate<double> value)
  : OptimizedScalar (1)
{
  set_value (value);
  value_name = "value";
}

//! Return the name of the class
string Model::ScalarValue::get_name () const
{
  return "ScalarValue";
}

//! Return the name of the specified parameter
string Model::ScalarValue::get_param_name (unsigned index) const
{
  if (index == 0)
    return value_name;
  else
    return "ERROR";
}

void Model::ScalarValue::set_value (const Estimate<double>& value)
{
  set_Estimate (0, value);
}

Estimate<double> Model::ScalarValue::get_value () const
{
  return get_Estimate (0);
}

string Model::ScalarValue::get_value_name () const
{
  return value_name;
}

void Model::ScalarValue::set_value_name (const string& name)
{
  value_name = name;
}

//! Return the value (and gradient, if requested) of the function
void Model::ScalarValue::calculate (double& result, vector<double>* grad)
{
  result = get_param(0);

  if (verbose) 
    cerr << "Model::ScalarValue::calculate result\n"
         "   " << result << endl;
 
  if (!grad)
    return;

  grad->resize (1);
  (*grad)[0] = 1.0;

  if (verbose)  {
    cerr << "Model::ScalarValue::calculate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}

