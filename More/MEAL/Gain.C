#include "Calibration/Gain.h"

Calibration::Gain::Gain () : OptimizedComplex2 (1)
{
  set_param (0, 1.0);
}

//! Return the name of the class
string Calibration::Gain::get_name () const
{
  return "Gain";
}

//! Return the name of the specified parameter
string Calibration::Gain::get_param_name (unsigned index) const
{
  if (index == 0)
    return "gain";
  else
    return "ERROR";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
void Calibration::Gain::calculate (Jones<double>& result,
				   vector<Jones<double> >* grad)
{
  double gain = get_param(0);

  if (verbose)
    cerr << "Calibration::Gain::calculate gain=" << gain << endl;

  result = gain;

  if (grad) {
    (*grad)[0] = 1.0;
    
    if (verbose)
      cerr << "Calibration::Gain::calculate gradient" << endl
	   << "   " << (*grad)[0] << endl;
  }
  
}

