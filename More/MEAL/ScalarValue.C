#include "MEAL/ScalarValue.h"

using namespace std;

MEAL::ScalarValue::ScalarValue (Estimate<double> value)
  : OptimizedScalar (1)
{
  set_value (value);
  value_name = "value";
}

//! Return the name of the class
string MEAL::ScalarValue::get_name () const
{
  return "ScalarValue";
}

//! Return the name of the specified parameter
string MEAL::ScalarValue::get_param_name (unsigned index) const
{
  if (index == 0)
    return value_name;
  else
    return "ERROR";
}

void MEAL::ScalarValue::set_value (const Estimate<double>& value)
{
  set_Estimate (0, value);
}

Estimate<double> MEAL::ScalarValue::get_value () const
{
  return get_Estimate (0);
}

string MEAL::ScalarValue::get_value_name () const
{
  return value_name;
}

void MEAL::ScalarValue::set_value_name (const string& name)
{
  value_name = name;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::ScalarValue::calculate (double& result, std::vector<double>* grad)
{
  result = get_param(0);

  if (verbose) 
    cerr << "MEAL::ScalarValue::calculate result\n"
         "   " << result << endl;
 
  if (!grad)
    return;

  grad->resize (1);
  (*grad)[0] = 1.0;

  if (verbose)  {
    cerr << "MEAL::ScalarValue::calculate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}

