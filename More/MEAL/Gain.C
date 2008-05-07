/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Gain.h"
#include "MEAL/OneParameter.h"

using namespace std;

MEAL::Gain::Gain ()
{
  OneParameter* param = new OneParameter (this);
  param->set_name ("gain");
  param->set_param (1.0);
}

//! Set the gain
void MEAL::Gain::set_gain (const Estimate<double>& gain)
{
  set_Estimate (0, gain);
}

//! Get the gain
Estimate<double> MEAL::Gain::get_gain () const
{
  return get_Estimate (0);
}

//! Return the name of the class
string MEAL::Gain::get_name () const
{
  return "Gain";
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

