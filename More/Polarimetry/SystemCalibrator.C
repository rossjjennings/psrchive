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

  set_calibrator(archive);
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
Pulsar::SystemCalibrator::get_CalibratorStokes () const
{
  if (!calibrator_stokes)
    throw Error (InvalidState,
		 "Pulsar::SystemCalibrator::get_CalibratorStokes",
		 "no CalibratorStokesExtension");

  return calibrator_stokes;
}

Pulsar::Archive*
Pulsar::SystemCalibrator::new_solution (const string& archive_class) const
{
  if (verbose) cerr << "Pulsar::SystemCalibrator::new_solution"
		 " create CalibratorStokes Extension" << endl;

  try {

    Reference::To<CalibratorStokes> stokes = get_CalibratorStokes();

    Reference::To<Archive> output;

    output = Calibrator::new_solution (archive_class);
    
    output -> add_extension (stokes);
    
    return output.release();

  }
  catch (Error& error) {
    throw error += "Pulsar::SystemCalibrator::new_solution";
  }

}

