#include "MEAL/Phase.h"

MEAL::Phase::Phase () : OptimizedComplex2 (1)
{
}

//! Return the name of the class
std::string MEAL::Phase::get_name () const
{
  return "Phase";
}

//! Return the name of the specified parameter
std::string MEAL::Phase::get_param_name (unsigned index) const
{
  if (index == 0)
    return "phase";
  else
    return "ERROR";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
void MEAL::Phase::calculate (Jones<double>& result,
				    std::vector<Jones<double> >* grad)
{
  double phase = get_param(0);

  if (verbose)
    std::cerr << "MEAL::Phase::calculate phase=" << phase << std::endl;

  double cos_phase = cos(phase);
  double sin_phase = sin(phase);

  result = std::complex<double>(cos_phase, sin_phase);

  if (grad) {
    (*grad)[0] = std::complex<double>(-sin_phase, cos_phase);
    
    if (verbose)
      std::cerr << "MEAL::Phase::calculate gradient" << std::endl
	   << "   " << (*grad)[0] << std::endl;
  }
  
}

