#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

Pulsar::CalibratorStokes*
Pulsar::ReceptionCalibrator::get_calibrator_stokes () const
{
  if (calibrator_stokes)
    return calibrator_stokes;

  if (verbose) cerr << "Pulsar::ReceptionCalibrator::get_calibrator_stokes"
		 " create CalibratorStokes Extension" << endl;

  unsigned nchan = get_transformation_nchan();

  if (nchan != calibrator_estimate.source.size())
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::get_calibrator_stokes",
		 "Calibrator Stokes nchan=%d != Transformation nchan=%d",
		 calibrator_estimate.source.size(), nchan);

  try {

    Reference::To<CalibratorStokes> ext = new CalibratorStokes;
    
    ext->set_nchan (nchan);
    
    for (unsigned ichan=0; ichan < nchan; ichan++) {
      
      bool valid = get_transformation_valid(ichan);
      
      ext->set_valid (ichan, valid);
      if (!valid)
	continue;

      ext->set_stokes (ichan, calibrator_estimate.source[ichan].get_stokes());
            
    }

    const_cast<ReceptionCalibrator*>(this)->calibrator_stokes = ext;

    return calibrator_stokes;

  }
  catch (Error& error) {
    throw error += "Pulsar::ReceptionCalibrator::get_calibrator_stokes";
  }

}

