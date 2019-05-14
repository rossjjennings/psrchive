/***************************************************************************
 *
 *   Copyright (C) 2003 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorStokesInfo.h"
#include "Pulsar/CalibratorStokes.h"

#include <assert.h>

//! Constructor
Pulsar::CalibratorStokesInfo::CalibratorStokesInfo (const CalibratorStokes* cs)
{
  calibrator_stokes = cs;
  together = false;
  degree = false;
}

//! Return the number of frequency channels
unsigned Pulsar::CalibratorStokesInfo::get_nchan () const
{
  return calibrator_stokes->get_nchan();
}

std::string Pulsar::CalibratorStokesInfo::get_title () const
{
  return "Reference Source Stokes Parameters";
} 

//! Return the number of parameter classes
unsigned Pulsar::CalibratorStokesInfo::get_nclass () const
{
  if (together)
    return 1;
  else
    return 3 + unsigned(degree);
}

//! Return the name of the specified class
std::string Pulsar::CalibratorStokesInfo::get_name (unsigned iclass) const
{
  // degree of polarization
  if (degree && iclass == 3)
    return "|\\fiC\\fr| (\%\\fiC\\fr\\d0\\u)";
  
  std::string label = "\\fiC\\fr\\dk\\u (\%\\fiC\\fr\\d0\\u)";
  if (!together)
  {
    std::string::size_type index = label.find('k');
    assert (index != std::string::npos);
    label[index] = '1' + iclass;
  }
  
  return label;
}


//! Return the number of parameters in the specified class
unsigned Pulsar::CalibratorStokesInfo::get_nparam (unsigned iclass) const
{
  if (together)
    return 3;
  else
    return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::CalibratorStokesInfo::get_param (unsigned ichan, unsigned iclass,
					 unsigned iparam) const
{
  if (!calibrator_stokes->get_valid (ichan))
    return 0.0;

  unsigned index = 0;

  if (together)
    index = iparam;
  else
    index = iclass;

  Estimate<double> value = 0;
  Stokes< Estimate<double> > stokes = calibrator_stokes->get_stokes(ichan);
  
  // degree of polarization
  if (degree && iclass == 3)
  {
    for (unsigned i=1; i<4; i++)
      value += stokes[i] * stokes[i];
    value = sqrt(value);
  }
  else
    value = stokes[index+1];

  return 100.0 * value;
}

