/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InterpolatedCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"

using namespace std;

Pulsar::InterpolatedCalibrator::InterpolatedCalibrator (Archive* data)
{
  interpolator = data->get<CalibrationInterpolatorExtension> ();
  if (!interpolator)
    throw Error (InvalidParam, "InterpolatedCalibrator ctor",
		 data->get_filename()
		 + " does not contain a CalibrationInterpolatorExtension");

  bool has_feedpar = false;  // has feed parameters
  bool has_calpol = false;   // has calibrator stokes parameters
  
  unsigned nparam = interpolator->get_nparam();

  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    auto param = interpolator->get_parameter (iparam);
    if (param->get_code() == "FEEDPAR")
      has_feedpar = true;
    else if (param->get_code() == "CAL_POLN")
      has_calpol = true;
  }

  if (!has_feedpar)
    throw Error (InvalidParam, "InterpolatedCalibrator ctor",
		 "CalibrationInterpolatorExtension has no feed parameters");

  poln_extension = feedpar = data->getadd<PolnCalibratorExtension> ();
  feedpar->set_type (interpolator->get_type());
  
  if (has_calpol)
  {
    calpoln = data->getadd<CalibratorStokes> ();
    calpoln->set_coupling_point (interpolator->get_coupling_point());
  }
  
  // store the calibrator archive
  set_calibrator (data);
  filenames.push_back( data->get_filename() );
}


//! Destructor
Pulsar::InterpolatedCalibrator::~InterpolatedCalibrator ()
{
}

MJD Pulsar::InterpolatedCalibrator::get_epoch () const
{
  return interpolator->get_reference_epoch();
}

void Pulsar::InterpolatedCalibrator::calculate_transformation ()
{

  unsigned target_nchan = observation_nchan;

  /*
    plan to hand over to PolnCalibration base class after setting up
    the poln_extension and reference_source attributes
  */
  
  // transformation.resize (target_nchan);

  for (unsigned ichan=0; ichan<target_nchan; ++ichan) try
  {
    if (verbose > 2)
      cerr << "Pulsar::InterpolatedCalibrator::calculate_transformation"
	" ichan=" << ichan << endl;


  }
  catch (Error& error)
  {
    if (verbose > 1)
      cerr << "Pulsar::InterpolatedCalibrator::calculate_transformation"
	" error ichan=" << ichan << error << endl;

  }

  if (verbose > 2)
    cerr << "Pulsar::InterpolatedCalibrator::calculate_transformation exit"
	 << endl;
}

