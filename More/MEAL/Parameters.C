#include "Calibration/Parameters.h"


//! Default constructor
Calibration::Parameters::Parameters (unsigned nparam)
  : params(nparam), fit(nparam, true)
{
}


//! Copy constructor
Calibration::Parameters::Parameters (const Parameters& np)
  : params(np.params), fit(np.fit)
{
}


//! Equality operator
Calibration::Parameters& 
Calibration::Parameters::operator = (const Parameters& np)
{
  if (&np == this)
    return *this;

  if (very_verbose)
    cerr << "Calibration::Parameters::operator= nparam=" << get_nparam()
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
void Calibration::Parameters::set_param (unsigned index, double value)
{
  range_check (index, "Calibration::Parameters::set_param");
  
  if (very_verbose) cerr << "Calibration::Parameters::set_param "
		 "(" << index << "," << value << ")" << endl;
  
  if (params[index].val == value)
    return;

  params[index].val = value;
  set_evaluation_changed ();
}


void Calibration::Parameters::resize (unsigned nparam)
{
  if (very_verbose)
    cerr << "Calibration::Parameters::resize " << nparam << endl;
  
  unsigned current = params.size();
  params.resize (nparam);
  fit.resize (nparam);
  for (; current < nparam; current++)
    fit[current] = true;
  
  changed.send (ParameterCount);
}

