#include "MEAL/CyclicParameter.h"

using namespace std;

MEAL::CyclicParameter::CyclicParameter (Function* context)
  : OneParameter (context)
{
  period = 2*M_PI;
  lower_bound = -M_PI;
  upper_bound = M_PI;
}


MEAL::CyclicParameter::CyclicParameter (const CyclicParameter& p)
  : OneParameter (p),
    period(p.period), 
    lower_bound(p.lower_bound),
    upper_bound(p.upper_bound)
{
}

//! Clone construtor
MEAL::CyclicParameter* MEAL::CyclicParameter::clone (Function* context) const
{
  MEAL::CyclicParameter* retval = new CyclicParameter (context);
  *retval = *this;
  return retval;
}

MEAL::CyclicParameter& 
MEAL::CyclicParameter::operator = (const CyclicParameter& p)
{
  if (&p == this)
    return *this;

  OneParameter::operator= (p);

  upper_bound = p.upper_bound;
  lower_bound = p.lower_bound;
  period = p.period;

  return *this;
}

//! Set the lower bound on the parameter value
void MEAL::CyclicParameter::set_lower_bound (double bound)
{
  lower_bound = bound;
  check (get_param(0));
}

//! Set the upper bound on the parameter value
void MEAL::CyclicParameter::set_upper_bound (double bound)
{
  upper_bound = bound;
  check (get_param(0));
}

//! Set the period of the parameter value
void MEAL::CyclicParameter::set_period (double period)
{
  period = period;
  check (get_param(0));
}

//! Set the value of the specified parameter
void MEAL::CyclicParameter::set_param (unsigned index, double value)
{
  check (value);
}

void MEAL::CyclicParameter::check (double value)
{
  if (value > upper_bound) {
    unsigned turns = unsigned((value-upper_bound)/period) + 1;
    value -= turns * period;
  }

  if (value < lower_bound) {
    unsigned turns = unsigned((lower_bound-value)/period) + 1;
    value += turns * period;
  }

  OneParameter::set_param (0, value);
}
