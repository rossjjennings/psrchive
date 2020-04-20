/***************************************************************************
 *
 *   Copyright (C) 2003 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConstantGainInfo.h"
#include "Pulsar/FluxCalibratorPolicy.h"

using namespace std;

string Pulsar::ConstantGainInfo::get_title () const
{
  return "Flux Density Scale Factor and Gain Ratio";
}

//! Return the number of frequency channels
unsigned Pulsar::ConstantGainInfo::get_nchan () const
{
  return instance->get_nchan();
}

//! Return the name of the specified class
std::string Pulsar::ConstantGainInfo::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "Scale (Jy)";
  case 1:
    return "Gain Ratio (g\\dH\\u/g\\dL\\u)";
  default:
    return "";
  }
}
    
//! Return the number of parameters in the specified class
unsigned Pulsar::ConstantGainInfo::get_nparam (unsigned iclass) const
{
  return instance->get_nreceptor();
}
    
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::ConstantGainInfo::get_param (unsigned ichan,
						     unsigned iclass,
						     unsigned iparam) const
{
  const FluxCalibrator::Policy* policy = instance->data[ichan];
  const FluxCalibrator::ConstantGain* cg
    = dynamic_cast<const FluxCalibrator::ConstantGain*> (policy);

  if (!cg)
    throw Error (InvalidState, "Pulsar::ConstantGainInfo::get_param"
		 "FluxCalibrator::Policy is not of type ConstantGain");

  Estimate<float> retval;

  if (iclass == 0)
    retval = 1.0e-3 * cg->get_gain(iparam);  // convert mJy to Jy
  else if (iclass == 1)
    retval = cg->get_gain_ratio(iparam);

  return retval;
}

