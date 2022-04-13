/***************************************************************************
 *
 *   Copyright (C) 2003 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorInfo.h"
#include "Pulsar/FluxCalibratorPolicy.h"

using namespace std;

string Pulsar::FluxCalibrator::Info::get_title () const
{
  return "System and Noise Source Equivalent Flux Densities";
}

//! Return the number of frequency channels
unsigned Pulsar::FluxCalibrator::Info::get_nchan () const
{
  return instance->get_nchan();
}

//! Return the name of the specified class
std::string Pulsar::FluxCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "\\fiS\\dcal\\u\\fr (Jy)";
  case 1:
    return "\\fiS\\dsys\\u\\fr (Jy)";
  default:
    return "";
  }
}
    
//! Return the number of parameters in the specified class
unsigned Pulsar::FluxCalibrator::Info::get_nparam (unsigned iclass) const
{
  return instance->get_nreceptor();
}
    
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::FluxCalibrator::Info::get_param (unsigned ichan,
							 unsigned iclass,
							 unsigned iparam) const
{
  Estimate<float> retval;

  if (! instance->get_valid(ichan) )
  {
    if (verbose > 2) 
      cerr << "Pulsar::FluxCalibrator::Info::get_param"
              " invalid ichan=" << ichan << endl;
    return 0;
  }

  if (iclass == 0)
    retval = instance->data[ichan]->get_S_cal(iparam);
  else if (iclass == 1)
    retval = instance->data[ichan]->get_S_sys(iparam);
  
  // convert to Jy
  retval *= 1e-3;
  return retval;
}
