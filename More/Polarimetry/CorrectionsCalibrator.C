#include "Pulsar/CorrectionsCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"

#include "Calibration/Parallactic.h"
#include "Pauli.h"

//! Default constructor
Pulsar::CorrectionsCalibrator::CorrectionsCalibrator () {}

//! Destructor
Pulsar::CorrectionsCalibrator::~CorrectionsCalibrator () {}

//! Return true if the archive needs to be corrected
bool Pulsar::CorrectionsCalibrator::needs_correction (const Archive* archive,
						      const Pointing* pointing)
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

  // a horizon mounted antenna that did not track equatorial PA ...
  should_correct_vertical = 
    telescope->get_mount() == Telescope::Horizon &&
    receiver->get_tracking_mode() != Receiver::Celestial;

  // ... or the angle tracked by the receiver is not zero
  if (pointing) {
    if (Archive::verbose == 3)
      cerr << "Pulsar::CorrectionsCalibrator::needs_correction"
	" using Pointing::pos_ang=" << pointing->pos_ang << endl;
    should_correct_vertical |= pointing->pos_ang != 0.0;
  }
  else
    should_correct_vertical |= receiver->get_tracking_angle () != 0.0;

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
    !receiver->get_feed_corrected() && should_correct_receptors;

  // return true if feed or platform needs correction
  return must_correct_feed || must_correct_platform;

}

void Pulsar::CorrectionsCalibrator::calibrate (Archive* archive)
{
  if (!archive->get_nsubint()) {
    if (Archive::Archive::verbose == 3)
      cerr << "Pulsar::CorrectionsCalibrator no data to correct" << endl;
    return;
  }

  if (!needs_correction (archive)) {
    if (Archive::Archive::verbose == 3)
      cerr << "Pulsar::CorrectionsCalibrator no corrections required" << endl;
    return;
  }

  if (!archive->get_poln_calibrated() && Archive::verbose) 
    cerr << "Pulsar::CorrectionsCalibrator " 
      "WARNING: correcting without calibrator" << endl;

  unsigned nsub = archive->get_nsubint();

  for (unsigned isub=0; isub < nsub; isub++) {

    Integration* integration = archive->get_Integration (isub);
      
    Jones<float> xform = inv( get_transformation (archive, isub) );
      
    Calibrator::calibrate (integration, xform);

  }

  if (must_correct_feed)
    receiver->set_feed_corrected (true);

  if (must_correct_platform)
    receiver->set_platform_corrected (true);

}

bool equal_pi (const Angle& a, const Angle& b, float tolerance = 0.01)
{
  // map 0->pi onto 0->1
  double ar = a.getRadians()/M_PI;
  double br = b.getRadians()/M_PI;

  // periodic map onto 0->1
  ar -= floor (ar);
  br -= floor (br);

  return fabs (ar - br) < tolerance;
}

//! Return the transformation matrix for the given epoch
Jones<double> 
Pulsar::CorrectionsCalibrator::get_transformation (const Archive* archive,
						   unsigned isub)
{
  // the identity matrix
  Jones<double> xform = 1;

  const Integration* integration = archive->get_Integration (isub);

  const Pointing* pointing = integration->get<Pointing>();

  if (pointing && !equal_pi (pointing->pos_ang,
			     pointing->fd_ang + pointing->par_ang) )  {

    // verify self-consistency of attributes

    if (Archive::verbose)
      cerr << "Pulsar::CorrectionsCalibrator::get_transformation WARNING\n"
	"  Pointing pos_ang=" << pointing->pos_ang << " != fd_ang+par_ang="
	   << pointing->fd_ang + pointing->par_ang << endl;
    
    pointing = 0;
    
  }

  if (!needs_correction( archive, pointing )) {
    if (Archive::verbose == 3)
      cerr << "Pulsar::CorrectionsCalibrator no corrections required" << endl;
    return xform;
  }

  Pauli::basis.set_basis( receiver->get_basis() );

  if (must_correct_feed)
    xform *= receiver->get_correction();

  double feed_rotation = 0.0;

  if (pointing)
    feed_rotation = pointing->pos_ang.getRadians();
  else
    feed_rotation = receiver->get_tracking_angle().getRadians();

  if (feed_rotation != 0.0) {

    // rotate the basis about the Stokes V axis
    Calibration::Rotation rotation ( Pauli::basis.get_basis_vector(2) );
    rotation.set_phi ( -feed_rotation );
    
    xform *= rotation.evaluate();

  }

  if (must_correct_platform && should_correct_projection)
    throw Error (InvalidState, "Pulsar::CorrectionsCalibrator::calibrate",
		 "Projection of fixed receptors not yet implemented");

  if (must_correct_platform && should_correct_vertical) {

    Calibration::Parallactic para;

    para.set_observatory_coordinates (telescope->get_latitude().getDegrees(),
				      telescope->get_longitude().getDegrees());

    para.set_source_coordinates( archive->get_coordinates() );

    para.set_epoch( integration->get_epoch() );

    if (pointing) {

      // check that the para_ang is equal
      if (!equal_pi( pointing->par_ang, -para.get_phi() ))
	
	if (Archive::verbose)
	  cerr << "Pulsar::CorrectionsCalibrator::get_transformation WARNING\n"
	    "  Pointing par_ang=" << pointing->par_ang << " != "
	       << -para.get_phi() << " calculated for MJD="
	       << integration->get_epoch() << endl;

    }
    else
      xform *= para.evaluate();
     
  }

  return xform;

}

