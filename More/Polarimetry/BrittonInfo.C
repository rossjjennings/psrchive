/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
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
}

string Pulsar::BrittonInfo::get_title () const
{
  return "Parameterization: Britton (2000) Equation 18";
}

//! Return the name of the specified class
string Pulsar::BrittonInfo::get_name_feed (unsigned iclass) const
{ 
  switch (iclass)
  {
  case 0:
    return "\\fi\\gd\\d\\gh,\\gc\\u\\fr (hrad.)";
  case 1:
    return "\\fi\\gs\\d\\gh,\\gc\\u\\fr (deg.)";
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

  Estimate<double> angle = xform->get_Estimate (3 + iclass*2 + iparam);

  if (iclass == 1)
    return 180.0 / M_PI * angle;
  else
    return angle;
}
