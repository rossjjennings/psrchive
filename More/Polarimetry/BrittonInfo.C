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

  for (unsigned iparam=0; iparam<nparam; iparam++)
    available_data[iparam] = false;

  for (unsigned ichan=0; ichan < calibrator->get_nchan(); ichan++)
  {
    if (!calibrator->get_transformation_valid (ichan))
      continue;

    const MEAL::Complex2* xform = calibrator->get_transformation (ichan);

    auto* bri00 = dynamic_cast<const Calibration::Britton2000*> (xform);
    if (!bri00)
      throw Error (InvalidState, "Pulsar::BrittonInfo::get_param_feed"
		   "xform is not of type Britton2000");

    if (bri00->get_degeneracy_isolated())
      degeneracy_isolated = true;

    for (unsigned iparam=0; iparam<nparam; iparam++)
      available_data[iparam] |= bri00->get_infit(iparam);
  }
}

string Pulsar::BrittonInfo::get_title () const
{
  string isolated;
  if (degeneracy_isolated)
    isolated = " with Isolation";
  return "Parameterization: Britton (2000) Equation 19" + isolated;
}

void add_with_comma_if_needed (string& total, const string& txt)
{
  if (total != "")
    total += ",";

  total += txt;
}

string Pulsar::BrittonInfo::get_param_name_feed (unsigned iparam) const
{
  string subscript[2];
  subscript[0] = "\\gh"; // theta
  subscript[1] = "\\gx"; // chi

  switch (iparam / 2)
  {
  case 0:
    return "\\fi\\gd\\d" + subscript[iparam % 2]  + "\\u\\fr";  // delta_{theta,chi}
  case 1:
    return "\\fi\\gs\\d" + subscript[!(iparam % 2)] + "\\u\\fr";  // sigma_{chi,theta}
  default:
    return "";
  }
}

string Pulsar::BrittonInfo::get_label_feed (unsigned iclass) const
{
  string differences;

  // b_u(delta_theta) was measured
  if (available_data[3])
    add_with_comma_if_needed(differences, "\\gh");

  // b_v(delta_chi) was measured
  if (available_data[4])
    add_with_comma_if_needed(differences, "\\gx");

  string sums;

  // r_u(sigma_chi) was measured
  if (available_data[5])
    add_with_comma_if_needed(sums, "\\gx");

  // r_v(sigma_theta) was measured
  if (available_data[6])
    add_with_comma_if_needed(sums, "\\gh");

  switch (iclass)
  {
  case 0:
    // italic-font greek-delta subscript-theta,chi roman-font (degree symbol)
    return "\\fi\\gd\\d" + differences + "\\u\\fr (\\(2729))";
  case 1:
    // italic-font greek-sigma subscript-chi,theta roman-font
    return "\\fi\\gs\\d" + sums + "\\u\\fr (\\(2729))";
  default:
    return "";
  }
}


//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::BrittonInfo::get_param_feed (unsigned ichan, unsigned iclass, unsigned iparam) const
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
