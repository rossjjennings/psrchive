#include "MEPL/Phase.h"

Model::Phase::Phase () : OptimizedComplex2 (1)
{
}

//! Return the name of the class
string Model::Phase::get_name () const
{
  return "Phase";
}

//! Return the name of the specified parameter
string Model::Phase::get_param_name (unsigned index) const
{
  if (index == 0)
    return "phase";
  else
    return "ERROR";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
void Model::Phase::calculate (Jones<double>& result,
				    vector<Jones<double> >* grad)
{
  double phase = get_param(0);

  if (verbose)
    cerr << "Model::Phase::calculate phase=" << phase << endl;

  double cos_phase = cos(phase);
  double sin_phase = sin(phase);

  result = complex<double>(cos_phase, sin_phase);

  if (grad) {
    (*grad)[0] = complex<double>(-sin_phase, cos_phase);
    
    if (verbose)
      cerr << "Model::Phase::calculate gradient" << endl
	   << "   " << (*grad)[0] << endl;
  }
  
}

