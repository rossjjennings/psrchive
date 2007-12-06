/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/InstrumentInfo.h"

#include <assert.h>

using namespace std;

//! Constructor
Pulsar::InstrumentInfo::InstrumentInfo (const PolnCalibrator* calibrator) :
  SingleAxisCalibrator::Info (calibrator)
{
  if (Calibrator::verbose)
    cerr << "Pulsar::InstrumentInfo::InstrumentInfo" << endl;

  fixed_orientation = false;

  unsigned nchan = calibrator->get_nchan ();
  
  // find the first valid transformation
  const MEAL::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( calibrator->get_transformation_valid (ichan) ) {
      xform = calibrator->get_transformation (ichan);
      break;
    }

  instrument = dynamic_cast<const Calibration::Instrument*> (xform);

  if (!instrument)
    throw Error (InvalidParam, "Pulsar::InstrumentInfo::InstrumentInfo",
		 "no valid Instrument transformation in PolnCalibrator");

  if (instrument->get_orientation(0).var == 0)
  {
    cerr << "Pulsar::InstrumentInfo orientation[0] set to zero" << endl;
    fixed_orientation = true;
  }
}

//! Return the number of parameter classes
unsigned Pulsar::InstrumentInfo::get_nclass () const
{
  // two extra classes: ellipticities and orientations
  return SingleAxisCalibrator::Info::get_nclass() + 2;
}
    
//! Return the name of the specified class
string Pulsar::InstrumentInfo::get_name (unsigned iclass) const
{
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_name(iclass);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();
  
  switch (iclass)
  {
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
  if( ! calibrator->get_transformation_valid (ichan) )
    return 0;
 
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_param (ichan, iclass, iparam);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();

  const Calibration::Instrument* instrument
    = dynamic_cast<const Calibration::Instrument*> 
    ( calibrator->get_transformation (ichan) );

  if (!instrument)
    throw Error (InvalidState, "Pulsar::InstrumentInfo::get_param",
		 "transformation[%d] is not an Instrument", ichan);

  if (iclass < 2)
  {
    Estimate<double> angle;
    if (iclass == 1)
      angle = instrument->get_orientation(iparam);
    else
      angle = instrument->get_ellipticity(iparam);
  
    return 180.0 / M_PI * angle;
  }

  return 0;
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
