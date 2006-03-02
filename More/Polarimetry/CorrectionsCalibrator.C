#include "Pulsar/CorrectionsCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"

#include "Calibration/Parallactic.h"
#include "Pauli.h"

bool Pulsar::CorrectionsCalibrator::pointing_over_computed = false;

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
    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator::needs_correction\n"
	"   Pointing::position_angle=" 
           << pointing->get_position_angle().getDegrees() << " deg" << endl;
    should_correct_vertical |= pointing->get_position_angle() != 0.0;
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
    receiver->get_orientation() != 0 || receiver->get_hand() != Signal::Right;

  if (verbose > 2)
    cerr << "Pulsar::CorrectionsCalibrator::needs_correction"
      "\n  orientation=" << receiver->get_orientation() <<
      "\n  hand=" << receiver->get_hand() <<
      "\n  -> should_correct_receptors=" << should_correct_receptors << endl;

  must_correct_feed =
    !receiver->get_feed_corrected() && should_correct_receptors;

  if (verbose > 2)
    cerr << "Pulsar::CorrectionsCalibrator::needs_correction"
      "\n  feed_corrected=" << receiver->get_feed_corrected() <<
      "\n  should_correct_receptors=" << should_correct_receptors <<
      "\n  -> must_correct_feed=" << must_correct_feed << endl;

  // return true if feed or platform needs correction
  return must_correct_feed || must_correct_platform;

}

void Pulsar::CorrectionsCalibrator::calibrate (Archive* archive)
{
  if (!archive->get_nsubint()) {
    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator no data to correct" << endl;
    return;
  }

  if (!needs_correction (archive)) {
    if (verbose > 2)
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
Pulsar::CorrectionsCalibrator::get_feed_transformation (const Pointing* point,
							const Receiver* rcvr)
{
  if (verbose > 2)
    cerr << "Pulsar::CorrectionsCalibrator::get_feed_transformation" << endl;

  double feed_rotation = 0.0;

  if (point) {
    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator::get_feed_transformation\n"
        "   using Pointing::feed_angle="
           << point->get_feed_angle().getDegrees() << " deg" << endl;
    feed_rotation = point->get_feed_angle().getRadians();
  }
  else if (rcvr) {
    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator::get_feed_transformation\n"
        "   using Receiver::tracking_angle="
           << rcvr->get_tracking_angle().getDegrees() << " deg" << endl;
    feed_rotation = rcvr->get_tracking_angle().getRadians();
  }

  if (feed_rotation == 0.0)
    return Jones<double> (1.0);

  // rotate the basis about the Stokes V axis
  MEAL::Rotation rotation ( Pauli::basis.get_basis_vector(2) );
  rotation.set_phi ( feed_rotation );
  return rotation.evaluate();

}

//! Return the transformation matrix for the given epoch
Jones<double> 
Pulsar::CorrectionsCalibrator::get_feed_transformation (const Archive* arch,
							unsigned isub)
{
  const Integration* integration = arch->get_Integration (isub);
  const Receiver* receiver = arch->get<Receiver>();
  const Pointing* pointing = integration->get<Pointing>();

  if (!receiver)
    throw Error (InvalidState, "Pulsar::CorrectionsCalibrator",
		 "no Receiver extension available");

  Pauli::basis.set_basis( receiver->get_basis() );

  return get_feed_transformation (pointing, receiver);
}

//! Return the transformation matrix for the given epoch
Jones<double> 
Pulsar::CorrectionsCalibrator::get_transformation (const Archive* archive,
						   unsigned isub)
{
  if (verbose > 2)
    cerr << "Pulsar::CorrectionsCalibrator::get_transformation" << endl;

  // the identity matrix
  Jones<double> xform = 1;

  const Integration* integration = archive->get_Integration (isub);

  const Pointing* pointing = integration->get<Pointing>();

  if (pointing && verbose > 2)
    cerr << "Pulsar::CorrectionsCalibrator::get_transformation"
            " Archive has Pointing" << endl;

  if (pointing && !equal_pi (pointing->get_position_angle(),
			     pointing->get_feed_angle() 
                             + pointing->get_parallactic_angle()) )  {

    // verify self-consistency of attributes

    if (Archive::verbose)
      cerr << "Pulsar::CorrectionsCalibrator::get_transformation WARNING\n"
	"  Pointing position_angle=" << pointing->get_position_angle() 
           << " != feed_angle+parallactic_angle="
	   << pointing->get_feed_angle() + pointing->get_parallactic_angle()
	   << endl;
    
    pointing = 0;

  }

  if (!needs_correction( archive, pointing )) {
    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator no corrections required" << endl;
    return xform;
  }

  Pauli::basis.set_basis( receiver->get_basis() );

  if (must_correct_feed)  {
    Jones<double> jones = receiver->get_transformation();
    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator::get_transformation"
	" adding receiver transformation\n  " << jones << endl;
    xform *= jones;
  }

  Jones<double> jones = get_feed_transformation (pointing, receiver);
  if (verbose > 2)
    cerr << "Pulsar::CorrectionsCalibrator::get_transformation"
      " adding feed transformation\n  " << jones << endl;

  xform *= jones;

  if (must_correct_platform && should_correct_projection)
    throw Error (InvalidState, "Pulsar::CorrectionsCalibrator::calibrate",
		 "Projection of fixed receptors not yet implemented");

  if (must_correct_platform && should_correct_vertical) {

    Calibration::Parallactic para;

    para.set_observatory_coordinates (telescope->get_latitude().getDegrees(),
				      telescope->get_longitude().getDegrees());

    para.set_source_coordinates( archive->get_coordinates() );

    para.set_epoch( integration->get_epoch() );

    Angle pa = para.get_parallactic_angle();
 
    // check that the para_ang is equal

    if (pointing && !equal_pi( pointing->get_parallactic_angle(), pa ))  {

      if (Archive::verbose)
        cerr << "Pulsar::CorrectionsCalibrator::get_transformation WARNING\n"
	  "  Pointing parallactic angle="
	     << pointing->get_parallactic_angle().getDegrees() << "deg "
	     << " != " << pa.getDegrees() << "deg calculated for MJD="
	     << integration->get_epoch() << endl;
  
      if (pointing_over_computed)  {
        if (Archive::verbose)
          cerr << "  Using Pointing parallactic angle." << endl;
        para.set_parallactic_angle 
             (pointing->get_parallactic_angle().getRadians());
      }
      else if (Archive::verbose)
        cerr << "  Using calculated parallactic angle." << endl;

    }

    if (verbose > 2)
      cerr << "Pulsar::CorrectionsCalibrator::get_transformation"
	" adding vertical transformation\n  " << para.evaluate() << endl;
    
    xform *= para.evaluate();
     
  }

  return xform;

}

