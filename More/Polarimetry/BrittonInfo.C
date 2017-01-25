/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BrittonInfo.h"
#include "Pulsar/Britton2000.h"

using namespace std;

//! Constructor
Pulsar::BrittonInfo::BrittonInfo (const PolnCalibrator* calibrator) :
  BackendFeedInfo (calibrator)
{
  degeneracy_isolated = false;
  constant_orientation = true;
  
  for (unsigned ichan=0; ichan < calibrator->get_nchan(); ichan++)
  {
    if (!calibrator->get_transformation_valid (ichan))
      continue;
					     
    const MEAL::Complex2* xform = calibrator->get_transformation (ichan);

    const Calibration::Britton2000* bri00
      = dynamic_cast<const Calibration::Britton2000*> (xform);
    if (!bri00)
      throw Error (InvalidState, "Pulsar::BrittonInfo::get_param_feed"
		   "xform is not of type Britton2000");

    if (bri00->get_degeneracy_isolated())
      degeneracy_isolated = true;

    if (!bri00->get_constant_orientation())
      constant_orientation = false;
  }
}

string Pulsar::BrittonInfo::get_title () const
{
  string isolated;
  if (degeneracy_isolated)
    isolated = " with Isolation";
  return "Parameterization: Britton (2000) Equation 19" + isolated;
}

//! Return the name of the specified class
string Pulsar::BrittonInfo::get_name_feed (unsigned iclass) const
{
  string orientation;
  if (!constant_orientation)
    orientation = ",\\gh";
  
  switch (iclass)
  {
  case 0:
    // italic-font greek-delta subscript-theta,chi roman-font (degree symbol)
    return "\\fi\\gd\\d\\gh,\\gx\\u\\fr (\\(2729))";
  case 1:
    // italic-font greek-sigma subscript-chi,theta roman-font
    return "\\fi\\gs\\d\\gx" + orientation + "\\u\\fr (\\(2729))";
  default:
    return "";
  }
}


//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::BrittonInfo::get_param_feed (unsigned ichan, unsigned iclass,
				     unsigned iparam) const
{
  if (iclass >= 2)
    return 0;

  const MEAL::Complex2* xform = calibrator->get_transformation (ichan);

  if (degeneracy_isolated)
    std::swap (iclass, iparam);
  
  /*
    Note that the free parameters are equal to one half of the values
    shown in Equation 19 of Britton (2000).  For example,

    b_1 = \delta_theta / 2

    Therefore, the result is multiplied by two.
  */
  return xform->get_Estimate (3 + iclass*2 + iparam) * (2 * 180.0 / M_PI);
}
