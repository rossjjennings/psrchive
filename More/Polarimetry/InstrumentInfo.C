#include "Pulsar/InstrumentInfo.h"

//! Constructor
Pulsar::InstrumentInfo::InstrumentInfo (const PolnCalibrator* calibrator) :
  SingleAxisCalibrator::Info (calibrator)
{
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
    return "\\fi\\ge\\dk\\u\\fn (deg.)";
  case 1:
    return "\\fi\\gh\\dk\\u\\fn (deg.)";
  default:
    return "";
  }
}

//! Return the name of the specified class
float Pulsar::InstrumentInfo::get_scale (unsigned iclass) const
{
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_scale(iclass);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();
  
  return 180.0 / M_PI;
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
  
  return SingleAxisCalibrator::Info::get_param (ichan, iparam, iclass);
  
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
