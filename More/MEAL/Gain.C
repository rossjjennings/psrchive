#include "MEAL/Gain.h"

using namespace std;

MEAL::Gain::Gain () : OptimizedComplex2 (1)
{
  set_param (0, 1.0);
}

//! Return the name of the class
string MEAL::Gain::get_name () const
{
  return "Gain";
}

//! Return the name of the specified parameter
string MEAL::Gain::get_param_name (unsigned index) const
{
  if (index == 0)
    return "gain";
  else
    return "ERROR";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
void MEAL::Gain::calculate (Jones<double>& result,
				   std::vector<Jones<double> >* grad)
{
  double gain = get_param(0);

  if (verbose)
    cerr << "MEAL::Gain::calculate gain=" << gain << endl;

  result = gain;

  if (grad) {
    (*grad)[0] = 1.0;
    
    if (verbose)
      cerr << "MEAL::Gain::calculate gradient" << endl
	   << "   " << (*grad)[0] << endl;
  }
  
}

