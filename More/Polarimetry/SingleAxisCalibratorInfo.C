/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SingleAxisCalibratorInfo.h"

using namespace Pulsar;
using namespace std;

SingleAxisCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

string SingleAxisCalibrator::Info::get_title () const
{
  return "Polar Decomposition of Complex Gains";
}

unsigned SingleAxisCalibrator::Info::get_nclass () const
{
  return 3; 
}

std::string SingleAxisCalibrator::Info::get_param_name (unsigned iparam) const
{
  switch (iparam) {
  case 0:
    return "\\fiG\\fr";  // italic "G"
  case 1:
    return "\\gg";       // gamma
  case 2:
    return "\\gf";       // phi
  default:
    return "";
  }
} 

//! Return the name of the specified class
string SingleAxisCalibrator::Info::get_label (unsigned iclass) const
{
  string name = get_param_name(iclass);

  switch (iclass) {
  case 0:
    return name + " (\\fic\\fr\\d0\\u)";  // calibrator flux density, c_0
  case 1:
    return name + " (%)";                 // percentage
  case 2:
    return name + " (\\(2729))";          // degrees
  default:
    return "";
  }
} 
  
//! Return the number of parameters in the specified class
unsigned SingleAxisCalibrator::Info::get_nparam (unsigned iclass) const
{
  if (iclass < 3)
    return 1;
  return 0;
}

Estimate<float> SingleAxisCalibrator::Info::get_param (unsigned ichan, 
					       unsigned iclass,
					       unsigned iparam) const
{
  if (iclass == 0)
    return PolnCalibrator::Info::get_param (ichan, iclass, iparam);

  /*
    See Equation (25) of van Straten (2006) and the text that follows it.

    The SingleAxis model is parameterized by beta and this object provides gamma.
  */
  
  if (iclass == 1)
    return 100.0 * 
      ( exp( 2*PolnCalibrator::Info::get_param (ichan, iclass, iparam) ) - 1 );

  // iclass == 2
  return 180.0/M_PI * PolnCalibrator::Info::get_param (ichan, iclass, iparam);
}

Calibrator::Info* SingleAxisCalibrator::get_Info () const
{
  return new SingleAxisCalibrator::Info (this);
}

