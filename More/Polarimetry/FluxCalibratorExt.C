/***************************************************************************
 *
 *   Copyright (C) 2004 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/FluxCalibratorPolicy.h"
#include "Pulsar/FluxCalibrator.h"

#include "Pulsar/Integration.h"
#include "strutil.h"

#include <fstream>
#include <assert.h>

using namespace std;

//! Construct from a FluxCalibrator instance
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension
(const FluxCalibrator* calibrator) 
  : CalibratorExtension ("FluxCalibratorExtension")
{
  if (!calibrator)
    throw Error (InvalidParam, "Pulsar::FluxCalibratorExtension",
                 "null FluxCalibrator*");

  try {

    if (Archive::verbose > 2)
      cerr << "Pulsar::FluxCalibratorExtension(FluxCalibrator*)" << endl;

    CalibratorExtension::build (calibrator);

    unsigned nchan = calibrator->get_nchan();
    set_nchan (nchan);

    for (unsigned ichan=0; ichan < nchan; ichan++) try {
      calibrator->data[ichan]->get (S_sys[ichan], S_cal[ichan]);
    }
    catch (Error& error) {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FluxCalibratorExtension constructor ichan=" << ichan
             << "\n\t" << error.get_message() << endl;
    }

    if (!dynamic_cast<FluxCalibrator::ConstantGain*>(calibrator->policy.get()))
      return;

    for (unsigned ichan=0; ichan < nchan; ichan++) try
    {
      FluxCalibrator::Policy* policy = calibrator->data[ichan];
      FluxCalibrator::ConstantGain* cg
	= dynamic_cast<FluxCalibrator::ConstantGain*> (policy);

      if (!cg)
	throw Error (InvalidState,
		     "Pulsar::FluxCalibratorExtension (FluxCalibrator*)",
		     "Policy is not of type ConstantGain");

      cg->get_scale (scale[ichan]);
      cg->get_gain_ratio (ratio[ichan]);
    }
    catch (Error& error) {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FluxCalibratorExtension constructor ichan=" << ichan
             << "\n\t" << error.get_message() << endl;
    }

  }
  
  catch (Error& error) {
    throw error += "Pulsar::FluxCalibratorExtension (FluxCalibrator*)";
  }

}
