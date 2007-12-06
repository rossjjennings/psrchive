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
  fixed_orientation = false;
  variation_nparam = 0;

  unsigned nchan = calibrator->get_nchan ();
  
  // find the first valid transformation
  const MEAL::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( calibrator->get_transformation_valid (ichan) ) {
      xform = calibrator->get_transformation (ichan);
      break;
    }

  if (!xform)
    return;

  instrument = dynamic_cast<const Calibration::Instrument*> (xform);

  if (!instrument)
    return;

  // parameter 4 is the orientation of receptor 0
  if (xform->get_Estimate(4).var == 0)  {
    cerr << "Pulsar::InstrumentInfo orientation of receptor 0 set to zero" 
         << endl;
    fixed_orientation = true;
  }

  set_variation( "\\fiG\\fr(\\fit\\fr)", instrument->get_gain_variation() );
  set_variation( "\\gb(\\fit\\fr)", instrument->get_diff_gain_variation() );
  set_variation( "\\gf(\\fit\\fr)", instrument->get_diff_phase_variation() );
}

void Pulsar::InstrumentInfo::set_variation (const std::string& name,
					    const MEAL::Scalar* function)
{
  if (!function)
    return;

  variation.push_back( pair<string,const MEAL::Scalar*>( name, function ) );

  variation_nparam += function->get_nparam();
}

//! Return the number of parameter classes
unsigned Pulsar::InstrumentInfo::get_nclass () const
{
  // by default, two extra classes: ellipticities and orientations
  unsigned nclass = 2;

  // plot each of the time variation model parameters in separate panels
  for (unsigned i=0; i<variation.size(); i++)
    nclass += variation[i].second->get_nparam();

  return SingleAxisCalibrator::Info::get_nclass() + nclass;
}
    
//! Return the name of the specified class
string Pulsar::InstrumentInfo::get_name (unsigned iclass) const
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

    iclass -= 2;

    for (unsigned i=0; i<variation.size(); i++)
    {
      if (iclass < variation[i].second->get_nparam())
	return variation[i].first + " " + tostring(iclass);
      iclass -= variation[i].second->get_nparam();
    }
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

  iclass -= 2;

  if (iclass < variation_nparam)
    return 1;

  return 0;
}


static void add_variation (vector<const MEAL::Scalar*>& functions, 
			   const MEAL::Scalar* function)
{
  if (function)
    functions.push_back( function );
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::InstrumentInfo::get_param (unsigned ichan, unsigned iclass,
				   unsigned iparam) const
{
  
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_param (ichan, iclass, iparam);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();

  if (iclass < 2)
  {
    // unscramble the orientation and ellipticity
    iparam += SingleAxisCalibrator::Info::get_nclass();
  
    return 180.0 / M_PI * 
      PolnCalibrator::Info::get_param (ichan, iparam, iclass);
  }

  iclass -= 2;

  if( ! variation.size() )
    return 0;

  if( ! calibrator->get_transformation_valid (ichan) )
    return 0;

  const Calibration::Instrument* data
    = dynamic_cast<const Calibration::Instrument*> 
    ( calibrator->get_transformation (ichan) );

  vector<const MEAL::Scalar*> functions;

  add_variation( functions, data->get_gain_variation() );
  add_variation( functions, data->get_diff_gain_variation() );
  add_variation( functions, data->get_diff_phase_variation() );

  assert( functions.size() == variation.size() );

  for (unsigned i=0; i<variation.size(); i++)
  {
    if (iclass < variation[i].second->get_nparam())
    {
      if (iclass < functions[i]->get_nparam())
	return functions[i]->get_Estimate(iclass);
      else
	return 0;
    }
    iclass -= variation[i].second->get_nparam();
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
