#include "MEAL/Parameters.h"

using namespace std;

//! Default constructor
MEAL::Parameters::Parameters (unsigned nparam)
  : params(nparam), fit(nparam, true)
{
}


//! Copy constructor
MEAL::Parameters::Parameters (const Parameters& np)
  : params(np.params), fit(np.fit)
{
}


//! Equality operator
MEAL::Parameters& 
MEAL::Parameters::operator = (const Parameters& np)
{
  if (&np == this)
    return *this;

  if (very_verbose)
    cerr << "MEAL::Parameters::operator= nparam=" << get_nparam()
         << " new nparam=" << np.get_nparam() << endl;

  bool nparam_changed = np.params.size() != params.size();

  params = np.params;
  fit = np.fit;

  set_evaluation_changed ();

  if (nparam_changed)
    changed.send (ParameterCount);

  return *this;
}


//! Set the value of the specified parameter
void MEAL::Parameters::set_param (unsigned index, double value)
{
  range_check (index, "MEAL::Parameters::set_param");
  
  if (very_verbose) cerr << "MEAL::Parameters::set_param "
		 "(" << index << "," << value << ")" << endl;
  
  if (params[index].val == value)
    return;

  params[index].val = value;
  set_evaluation_changed ();
}


void MEAL::Parameters::resize (unsigned nparam)
{
  if (very_verbose)
    cerr << "MEAL::Parameters::resize " << nparam << endl;
  
  unsigned current = params.size();
  params.resize (nparam);
  fit.resize (nparam);
  for (; current < nparam; current++)
    fit[current] = true;
  
  changed.send (ParameterCount);
}

