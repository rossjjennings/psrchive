#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Archive.h"

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  SystemCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::SystemCalibrator::SystemCalibrator (Archive* archive)
{
  if (!archive)
    return;

  calibrator = archive;
  extension = archive->get<PolnCalibratorExtension>();
  calibrator_stokes = archive->get<CalibratorStokes>();
}

//! Copy constructor
Pulsar::SystemCalibrator::SystemCalibrator (const SystemCalibrator& calibrator)
{
}

//! Destructor
Pulsar::SystemCalibrator::~SystemCalibrator ()
{
}

//! Return the CalibratorStokes Extension
Pulsar::CalibratorStokes* 
Pulsar::SystemCalibrator::get_calibrator_stokes () const
{
  if (!calibrator_stokes)
    throw Error (InvalidState,
		 "Pulsar::SystemCalibrator::get_calibrator_stokes",
		 "no CalibratorStokesExtension");

  return calibrator_stokes;
}

Pulsar::Archive*
Pulsar::SystemCalibrator::get_solution (const string& archive_class,
					string filename_extension) const
{
  if (verbose) cerr << "Pulsar::SystemCalibrator::get_solution"
		 " create CalibratorStokes Extension" << endl;

  try {

    Reference::To<CalibratorStokes> ext = get_calibrator_stokes();

    Reference::To<Archive> output;

    output = PolnCalibrator::get_solution (archive_class, filename_extension);
    
    output -> add_extension (ext);
    
    return output.release();

  }
  catch (Error& error) {
    throw error += "Pulsar::SystemCalibrator::get_solution";
  }

}

