#include "MEAL/OneParameter.h"
#include "MEAL/Function.h"

using namespace std;

MEAL::OneParameter::OneParameter (Function* context)
  : ParameterPolicy(context), fit(true)
{
}


MEAL::OneParameter::OneParameter (const OneParameter& p)
  : ParameterPolicy (0), param(p.param), fit(p.fit), name(p.name)
{
}

//! Clone construtor
MEAL::OneParameter* MEAL::OneParameter::clone (Function* context) const
{
  MEAL::OneParameter* retval = new OneParameter (context);
  *retval = *this;
  return retval;
}

MEAL::OneParameter& 
MEAL::OneParameter::operator = (const OneParameter& np)
{
  if (&np == this)
    return *this;

  param = np.param;
  fit = np.fit;
  name = np.name;

  get_context()->set_evaluation_changed ();

  return *this;
}


//! Set the value of the specified parameter
void MEAL::OneParameter::set_param (double value)
{
  if (param.val == value)
    return;

  param.val = value;
  get_context()->set_evaluation_changed ();
}

