#include "Pulsar/CorrectionsCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"

#include "Calibration/Parallactic.h"
#include "Pauli.h"

//! Default constructor
Pulsar::CorrectionsCalibrator::CorrectionsCalibrator () {}

//! Destructor
Pulsar::CorrectionsCalibrator::~CorrectionsCalibrator () {}

//! Return true if the archive needs to be corrected
bool Pulsar::CorrectionsCalibrator::needs_correction (const Archive* archive)
{
  receiver = const_cast<Receiver*>( archive->get<Receiver>() );

  if (!receiver)
    throw Error (InvalidState, "Pulsar::CorrectionsCalibrator",
		 "no Receiver extension available");

  telescope = const_cast<Telescope*>( archive->get<Telescope>() );

  if (!telescope)
    throw Error (InvalidState, "Pulsar::CorrectionsCalibrator",
		 "no Telescope extension available");

  // determine if if is necessary to correct for known platform projections

  // a horizon mounted antenna that did not track equatorial PA
  should_correct_vertical = 
    telescope->get_mount() == Telescope::Horizon &&
    receiver->get_tracking_mode() != Receiver::Celestial;
  
  // a fixed antenna, such as a dipole array (or Arecibo?)
  should_correct_projection = 
    telescope->get_mount() == Telescope::Fixed;

  // note: the calibrator source is assumed to be fixed to the receiver
  must_correct_platform =
    !receiver->get_platform_corrected() && !archive->type_is_cal() &&
    (should_correct_vertical || should_correct_projection);

  // determine if it is necesary to correct for known receptor projections
  
  should_correct_receptors = 
    receiver->get_X_offset() != 0 || receiver->get_Y_offset() != 0;

  should_correct_calibrator =
    archive->type_is_cal() && receiver->get_calibrator_offset() != 0;

  must_correct_feed =
    !receiver->get_feed_corrected() &&
    (should_correct_receptors || should_correct_calibrator);
  
  return must_correct_feed || must_correct_platform;

}

void Pulsar::CorrectionsCalibrator::calibrate (Archive* archive)
{
  if (!archive->get_nsubint()) {
    if (Archive::verbose)
      cerr << "Pulsar::CorrectionsCalibrator no data to correct" << endl;
    return;
  }

  if (!needs_correction (archive)) {
    if (Archive::verbose)
      cerr << "Pulsar::CorrectionsCalibrator no corrections required" << endl;
    return;
  }

  if (!archive->get_poln_calibrated() && Archive::verbose) 
    cerr << "Pulsar::CorrectionsCalibrator " 
      "WARNING: correcting without calibrator" << endl;

  unsigned nsub = archive->get_nsubint();

  for (unsigned isub=0; isub < nsub; isub++) {

    Integration* integration = archive->get_Integration (isub);
    MJD epoch = integration->get_epoch ();
      
    Jones<float> xform = inv( get_transformation (archive, epoch) );
      
    Calibrator::calibrate (integration, xform);

  }

  // notice that the above loop combines the correction
  if (must_correct_feed)
    receiver->set_feed_corrected (true);

  receiver->set_platform_corrected (true);

  archive->set_state (archive->get_Integration(0)->get_state());

}


//! Return the transformation matrix for the given epoch
Jones<double> 
Pulsar::CorrectionsCalibrator::get_transformation (const Archive* archive,
						   const MJD& epoch)
{
  Jones<double> xform = Jones<double>::identity();

  if (!needs_correction (archive)) {
    if (Archive::verbose)
      cerr << "Pulsar::CorrectionsCalibrator no corrections required" << endl;
    return xform;
  }

  Pauli::basis.set_basis( receiver->get_basis() );

  if (must_correct_feed)
    xform *= receiver->get_correction();

  if (must_correct_platform && should_correct_projection)
    throw Error (InvalidState, "Pulsar::CorrectionsCalibrator::calibrate",
		 "Projection of fixed receptors not yet implemented");

  if (must_correct_platform && should_correct_vertical) {

    Calibration::Parallactic para;

    para.set_observatory_coordinates (telescope->get_latitude().getDegrees(),
				      telescope->get_longitude().getDegrees());

    para.set_source_coordinates( archive->get_coordinates() );

    para.set_epoch (epoch);

    xform *= para.evaluate();
     
  }

  return xform;

}

