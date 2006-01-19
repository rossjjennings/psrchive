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
  check (get_param());
}

//! Set the upper bound on the parameter value
void MEAL::CyclicParameter::set_upper_bound (double bound)
{
  upper_bound = bound;
  check (get_param());
}

//! Set the period of the parameter value
void MEAL::CyclicParameter::set_period (double P)
{
  period = P;
  check (get_param());
}

//! Set the matching azimuth and behave like an elevation
void MEAL::CyclicParameter::set_azimuth (CyclicParameter* az)
{
  azimuth = az;
  check (get_param());
}

//! Set the value of the specified parameter
void MEAL::CyclicParameter::set_param (double value)
{
  check (value);
}

void MEAL::CyclicParameter::check (double value)
{
  cerr << "MEAL::CyclicParameter::check value=" << value << endl;

  if (value > upper_bound) {
    cerr << "MEAL::CyclicParameter::check value=" << value 
	 << " > upper=" << upper_bound << endl;
    unsigned turns = unsigned((value-upper_bound)/period) + 1;
    cerr << "MEAL::CyclicParameter::check turns=" << turns << endl;
    value -= turns * period;
    cerr << "MEAL::CyclicParameter::check new value=" << value << endl;
  }

  if (value < lower_bound) {
    cerr << "MEAL::CyclicParameter::check value=" << value 
	 << " < lower=" << lower_bound << endl;
    unsigned turns = unsigned((lower_bound-value)/period) + 1;
    cerr << "MEAL::CyclicParameter::check turns=" << turns << endl;
    value += turns * period;
    cerr << "MEAL::CyclicParameter::check new value=" << value << endl;
  }

  cerr << "MEAL::CyclicParameter::check call OneParameter::set_param" << endl;

  OneParameter::set_param (value);
}

//! Get the value of the parameter
double MEAL::CyclicParameter::get_param () const
{
  if (azimuth)
    const_cast<CyclicParameter*>(this)->check_elevation ();

  return OneParameter::get_param ();
}

void MEAL::CyclicParameter::check_elevation ()
{
  double value = OneParameter::get_param ();

  cerr << "MEAL::CyclicParameter::check_elevation value=" << value << endl;
  if (value > upper_bound) {
    value = 0.5 * period - value;
    azimuth->set_param(azimuth->get_param() + 0.5 * azimuth->period);
    OneParameter::set_param (value);
  }
  if (value < lower_bound) {
    value = -0.5 * period - value;
    azimuth->set_param(azimuth->get_param() + 0.5 * azimuth->period);
    OneParameter::set_param (value);
  }
}
