#include "MEPL/Parameters.h"


//! Default constructor
Model::Parameters::Parameters (unsigned nparam)
  : params(nparam), fit(nparam, true)
{
}


//! Copy constructor
Model::Parameters::Parameters (const Parameters& np)
  : params(np.params), fit(np.fit)
{
}


//! Equality operator
Model::Parameters& 
Model::Parameters::operator = (const Parameters& np)
{
  if (&np == this)
    return *this;

  if (very_verbose)
    cerr << "Model::Parameters::operator= nparam=" << get_nparam()
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
void Model::Parameters::set_param (unsigned index, double value)
{
  range_check (index, "Model::Parameters::set_param");
  
  if (very_verbose) cerr << "Model::Parameters::set_param "
		 "(" << index << "," << value << ")" << endl;
  
  if (params[index].val == value)
    return;

  params[index].val = value;
  set_evaluation_changed ();
}


void Model::Parameters::resize (unsigned nparam)
{
  if (very_verbose)
    cerr << "Model::Parameters::resize " << nparam << endl;
  
  unsigned current = params.size();
  params.resize (nparam);
  fit.resize (nparam);
  for (; current < nparam; current++)
    fit[current] = true;
  
  changed.send (ParameterCount);
}

