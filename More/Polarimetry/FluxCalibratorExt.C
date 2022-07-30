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

// #include _DEBUG 1
#include "debug.h"

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

  scale_available = false;

  try {

    if (Archive::verbose > 2)
      cerr << "Pulsar::FluxCalibratorExtension(FluxCalibrator*)" << endl;

    CalibratorExtension::build (calibrator);

    unsigned nchan = calibrator->get_nchan();
    unsigned nreceptor = calibrator->get_nreceptor();

    if (Archive::verbose > 2)
      cerr << "FluxCalibratorExtension ctor nchan=" << nchan
           << " nreceptor=" << nreceptor << endl;

    set_nchan (nchan);
    set_nreceptor (nreceptor);

    for (unsigned ichan=0; ichan < nchan; ichan++) try
    {
      calibrator->data[ichan]->get (solution[ichan].S_sys, solution[ichan].S_cal);
      DEBUG("\t S_sys["<< ichan <<"].size=" << solution[ichan].S_sys.size());
    }
    catch (Error& error) {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FluxCalibratorExtension constructor ichan=" << ichan
             << "\n\t" << error.get_message() << endl;
    }

    if (!dynamic_cast<FluxCalibrator::ConstantGain*>(calibrator->policy.get()))
      return;

    if (Archive::verbose > 2)
      cerr << "FluxCalibratorExtension ctor scale available" << endl;

    scale_available = true;

    for (unsigned ichan=0; ichan < nchan; ichan++) try
    {
      FluxCalibrator::Policy* policy = calibrator->data[ichan];
      FluxCalibrator::ConstantGain* cg
	= dynamic_cast<FluxCalibrator::ConstantGain*> (policy);

      if (!cg)
	throw Error (InvalidState,
		     "Pulsar::FluxCalibratorExtension (FluxCalibrator*)",
		     "Policy is not of type ConstantGain");

      cg->get_scale (solution[ichan].scale);
      cg->get_gain_ratio (solution[ichan].ratio);
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

