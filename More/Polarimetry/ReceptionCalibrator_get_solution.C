#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

Pulsar::CalibratorStokes*
Pulsar::ReceptionCalibrator::get_CalibratorStokes () const
{
  if (calibrator_stokes)
    return calibrator_stokes;

  if (verbose > 2) cerr << "Pulsar::ReceptionCalibrator::get_CalibratorStokes"
		 " create CalibratorStokes Extension" << endl;

  unsigned nchan = get_transformation_nchan();

  if (nchan != calibrator_estimate.source.size())
    throw Error (InvalidState,
		 "Pulsar::ReceptionCalibrator::get_CalibratorStokes",
		 "Calibrator Stokes nchan=%d != Transformation nchan=%d",
		 calibrator_estimate.source.size(), nchan);

  Reference::To<CalibratorStokes> ext = new CalibratorStokes;
    
  ext->set_nchan (nchan);
    
  for (unsigned ichan=0; ichan < nchan; ichan++) try {
      
    bool valid = get_transformation_valid(ichan);
      
    ext->set_valid (ichan, valid);
    if (!valid)
      continue;
    
    ext->set_stokes (ichan, calibrator_estimate.source[ichan]->get_stokes());
    
  }
  catch (Error& error) {
    cerr << "Pulsar::ReceptionCalibrator::get_CalibratorStokes ichan="
	 << ichan << " error\n" << error.get_message() << endl;
    ext->set_valid (ichan, false);
  }

  const_cast<ReceptionCalibrator*>(this)->calibrator_stokes = ext;
  
  return calibrator_stokes;

}

