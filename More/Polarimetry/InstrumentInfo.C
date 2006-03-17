/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/InstrumentInfo.h"

//! Constructor
Pulsar::InstrumentInfo::InstrumentInfo (const PolnCalibrator* calibrator) :
  SingleAxisCalibrator::Info (calibrator)
{
  fixed_orientation = false;

  unsigned nchan = calibrator->get_transformation_nchan ();
  
  // find the first valid transformation
  const MEAL::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( calibrator->get_transformation_valid (ichan) ) {
      xform = calibrator->get_transformation (ichan);
      break;
    }

  if (!xform)
    return;

  // parameter 4 is the orientation of receptor 0
  if (xform->get_Estimate(4).var == 0)  {
    cerr << "Pulsar::InstrumentInfo orientation of receptor 0 set to zero" 
         << endl;
    fixed_orientation = true;
  }

}
    
//! Return the number of parameter classes
unsigned Pulsar::InstrumentInfo::get_nclass () const
{
  return SingleAxisCalibrator::Info::get_nclass() + 2;
}
    
//! Return the name of the specified class
const char* Pulsar::InstrumentInfo::get_name (unsigned iclass) const
{
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_name(iclass);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();
  
  switch (iclass) {
  case 0:
    return "\\fi\\ge\\dk\\u\\fr (deg.)";
  case 1: {
    if (fixed_orientation)
      return "\\fi\\gh\\fr\\d1\\u (deg.)";
    else
      return "\\fi\\gh\\dk\\u\\fr (deg.)";
  }
  default:
    return "";
  }
}


//! Return the number of parameters in the specified class
unsigned Pulsar::InstrumentInfo::get_nparam (unsigned iclass) const
{
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_nparam(iclass);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();
  
  if (iclass < 2)
    return 2;
  
  return 0;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::InstrumentInfo::get_param (unsigned ichan, unsigned iclass,
				   unsigned iparam) const
{
  
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_param (ichan, iclass, iparam);
  
  // unscramble the orientation and ellipticity
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();
  iparam += SingleAxisCalibrator::Info::get_nclass();
  
  return 180.0 / M_PI * 
    PolnCalibrator::Info::get_param (ichan, iparam, iclass);
}

//! Return the colour index
int Pulsar::InstrumentInfo::get_colour_index (unsigned iclass,
					      unsigned iparam) const
{
  if (iparam == 0)
    return 1;
  else
    return 2;
}
