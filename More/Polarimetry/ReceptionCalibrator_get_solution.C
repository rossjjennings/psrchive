#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

Pulsar::Archive*
Pulsar::ReceptionCalibrator::get_solution (const string& archive_class,
					   string filename_extension) const
{
  if (verbose) cerr << "Pulsar::ReceptionCalibrator::get_solution"
		 " create CalibratorStokes Extension" << endl;

  unsigned nchan = get_Transformation_nchan();

  if (nchan != calibrator_estimate.source.size())
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::get_solution",
		 "Calibrator Stokes nchan=%d != Transformation nchan=%d",
		 calibrator_estimate.source.size(), nchan);

  try {

    Reference::To<CalibratorStokes> ext = new CalibratorStokes;
    
    ext->set_nchan (nchan);
    
    for (unsigned ichan=0; ichan < nchan; ichan++) {
      
      bool valid = get_Transformation_valid(ichan);
      
      ext->set_valid (ichan, valid);
      if (!valid)
	continue;
      
      Stokes< Estimate<double> > s;
      calibrator_estimate.source[ichan].State::evaluate (s);
      
      ext->set_stokes (ichan, s);
      
    }
    
    Reference::To<Archive> output;
    output = PolnCalibrator::get_solution (archive_class, filename_extension);
    
    output -> add_extension (ext);
    
    return output.release();

  }
  catch (Error& error) {
    throw error += "Pulsar::ReceptionCalibrator::get_solution";
  }

}




