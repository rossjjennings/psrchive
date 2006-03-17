/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Phase.h"
#include "MEAL/OneParameter.h"

MEAL::Phase::Phase ()
{
  OneParameter* param = new OneParameter (this);
  param->set_param_name ("phase");
}

//! Return the name of the class
std::string MEAL::Phase::get_name () const
{
  return "Phase";
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

