/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
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

    if (dynamic_cast<FluxCalibrator::ConstantGain*>(calibrator->policy.get()))
    {
      std::string filename = calibrator->get_Archive()->get_filename();
      filename = replace_extension (filename, "gratio");

      cerr << "Unloading gain ratio data to " << filename << endl;
      
      std::ofstream out (filename.c_str());
      out << "# freq ratio0 ratio0_err ratio1 ratio1_err" << endl;
      
      const Integration* subint = calibrator->get_Archive()->get_Integration(0);
      
      for (unsigned ichan=0; ichan<get_nchan(); ++ichan) try
      {
	double frequency = subint->get_centre_frequency(ichan);

	FluxCalibrator::ConstantGain* cg;
	cg = dynamic_cast<FluxCalibrator::ConstantGain*> (calibrator->data[ichan].get());
	assert(cg != NULL);

	Estimate<double> gr0 = cg->get_gain_ratio(0);
	Estimate<double> gr1 = cg->get_gain_ratio(1);
	
	out << frequency
	    << " " << gr0.val << " " << sqrt(gr0.var)
	    << " " << gr1.val << " " << sqrt(gr1.var)
	    << endl;
      }
      catch (std::exception&)
      {
	// ignore bad channels
      }
    }
        
  }
  catch (Error& error) {
    throw error += "Pulsar::FluxCalibratorExtension (FluxCalibrator*)";
  }

}
