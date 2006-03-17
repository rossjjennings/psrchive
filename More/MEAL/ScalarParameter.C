/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarParameter.h"
#include "MEAL/OneParameter.h"

using namespace std;

MEAL::ScalarParameter::ScalarParameter (const Estimate<double>& value)
{
  new OneParameter (this);
  set_value (value);
  value_name = "value";
}

//! Return the name of the class
string MEAL::ScalarParameter::get_name () const
{
  return "ScalarParameter";
}

void MEAL::ScalarParameter::set_value (const Estimate<double>& value)
{
  set_Estimate (0, value);
}

Estimate<double> MEAL::ScalarParameter::get_value () const
{
  return get_Estimate (0);
}

string MEAL::ScalarParameter::get_value_name () const
{
  return value_name;
}

void MEAL::ScalarParameter::set_value_name (const string& name)
{
  value_name = name;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::ScalarParameter::calculate (double& result, 
                                       std::vector<double>* grad)
{
  result = get_param(0);

  if (verbose) 
    cerr << "MEAL::ScalarParameter::calculate result\n"
         "   " << result << endl;
 
  if (!grad)
    return;

  grad->resize (1);
  (*grad)[0] = 1.0;

  if (verbose)  {
    cerr << "MEAL::ScalarParameter::calculate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}

