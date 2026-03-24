/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProjectionCorrection.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"

#include "MountProjection.h"
#include "Pauli.h"

using namespace std;

bool Pulsar::ProjectionCorrection::trust_pointing_para_angle = false;
bool Pulsar::ProjectionCorrection::trust_pointing_feed_angle = false;

Pulsar::ProjectionCorrection::ProjectionCorrection ()
{
}

Pulsar::ProjectionCorrection::~ProjectionCorrection ()
{
}

//! Return true if the archive needs to be corrected
void Pulsar::ProjectionCorrection::set_archive (const Archive* _archive)
{
  archive.set(_archive);

  if (!archive)
    return;

  receiver.set(archive->get<Receiver>());
  if (!receiver)
    throw Error (InvalidState, "Pulsar::ProjectionCorrection::set_archive",
		 "no Receiver extension available");

  telescope.set(archive->get<Telescope>());
  if (!telescope)
    throw Error (InvalidState, "Pulsar::ProjectionCorrection::set_archive",
		 "no Telescope extension available");

  mount = mount_factory (telescope->get_mount());
  if (!mount)
    return;

  double lat = telescope->get_latitude().getRadians();
  double lon = telescope->get_longitude().getRadians();

  if (Archive::verbose > 2)
    cerr << "Pulsar::ProjectionCorrection::set_archive "
         << mount->get_name() << " mount \n"
            " antenna latitude=" << lat*180/M_PI << "deg"
            " longitude=" << lon*180/M_PI << "deg \n"
            " source coordinates=" << archive->get_coordinates() << endl;

  mount->set_observatory_latitude (lat);
  mount->set_observatory_longitude (lon);
  mount->set_source_coordinates ( archive->get_coordinates() );

  Directional* directional = dynamic_cast<Directional*> (mount.get());
  if (directional)
    para.set_directional (directional);

  projection = dynamic_cast<MountProjection*> (mount.get());
}

//! Return the projection correction calculator
Mount* Pulsar::ProjectionCorrection::get_mount ()
{
  if (!mount)
    throw Error (InvalidState, "ProjectionCorrection::get_mount",
		 "Mount attribute not set");
  
  return mount;
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

//! Return true if the archive needs to be corrected
bool Pulsar::ProjectionCorrection::required (unsigned isub) const try
{
  integration.set(archive->get_Integration (isub));
  pointing.set(integration->get<Pointing>());

  if (pointing && Archive::verbose > 2)
    cerr << "Pulsar::ProjectionCorrection::required has Pointing" << endl;

  if (pointing && !equal_pi (pointing->get_position_angle(),
                              pointing->get_feed_angle() 
                             + pointing->get_parallactic_angle()) )
  {
    // verify self-consistency of attributes

    if (Archive::verbose > 1)
      cerr << "Pulsar::ProjectionCorrection::get_transformation WARNING\n"
              "  Pointing position_angle=" << pointing->get_position_angle() 
           << " != feed_angle+parallactic_angle="
           << pointing->get_feed_angle() + pointing->get_parallactic_angle()
           << endl;

    if (trust_pointing_feed_angle)
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::ProjectionCorrection::required"
          "\n  temporarily set Pointing::position_angle = feed_angle" << endl;

      /*
        Normally, if the numbers don't add up, then it is assumed that they
        were never set to proper values and the Pointing data is ignored.

        If trust_pointing_feed_angle is set, then the feed angle was
        likely set to a non-zero value that should be applied.

        Temporarily set the pointing position angle to this value;
        it will be corrected to equal feed angle + parallactic angle
        following the comment "correct position angle"
      */
      const_kast(pointing)->set_position_angle( pointing->get_feed_angle() );
    }
    else
      pointing = 0;
  }

  // determine if it is necessary to correct for known platform projections

  // a non-celestial mount that did not track celestial position angle ...
  should_correct_vertical = 
    !naturally_celestial( telescope->get_mount() ) &&
    receiver->get_tracking_mode() != Receiver::Celestial;

  // ... or the angle tracked by the receiver is not zero
  if (pointing) 
  {
    if (Archive::verbose > 2)
    {
      cerr << "Pulsar::ProjectionCorrection::required"
              "\n  Pointing::position_angle=" 
           << pointing->get_position_angle().getDegrees() << " deg" << endl;
    }
    should_correct_vertical |= pointing->get_position_angle() != 0.0;
  }
  else
  {
    if (Archive::verbose > 2)
    {
      cerr << "Pulsar::ProjectionCorrection::required"
              "\n  Receiver::tracking_angle=" 
           << receiver->get_tracking_angle () << " deg" << endl;
    }
    should_correct_vertical |= receiver->get_tracking_angle () != 0.0;
  }

  // a fixed antenna, such as a dipole array
  should_correct_projection = 
    telescope->get_mount() == Telescope::Fixed;

  // note: the calibrator source is assumed to be fixed to the receiver
  must_correct_platform =
    !receiver->get_projection_corrected() && !archive->type_is_cal() &&
    (should_correct_vertical || should_correct_projection);

  if (Archive::verbose > 2)
    cerr << "  Receiver::projection_corrected=" 
        << receiver->get_projection_corrected() 
        << "\n  should_correct_vertical=" << should_correct_vertical
        << "\n  should_correct_projection=" << should_correct_projection
        << "\n  -> must_correct_platform=" << must_correct_platform << endl;

  // return true if feed or platform needs correction
  return must_correct_platform;
}
catch (Error& error)
{
  throw error += "Pulsar::ProjectionCorrection::required";
}

Jones<double> get_los_rotation (const Angle& angle)
{
  if (angle == 0.0)
    return Jones<double> (1.0);

  /* The evaluate methods of the MEAL::Rotation and MEAL::Rotation1 classes
     return a passive / alias transformation that represents a change of basis.

     This is desired because the feed rotation angle is measured
     counter-clockwise from the axis that points toward celestial north
     to the receptor basis axis that points toward zenith.
   */

  // rotate the basis about the Stokes V axis

  MEAL::Rotation1 rotation ( Pauli::basis().get_basis_vector(2) );
  rotation.set_phi ( angle.getRadians() );
  return rotation.evaluate();
}

Jones<double> Pulsar::ProjectionCorrection::get_feed_projection () const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::ProjectionCorrection::get_feed_projection" << endl;

  Angle feed_rotation = 0.0;

  if (pointing)
  {
    string degrees = tostring(pointing->get_feed_angle().getDegrees());
    summary += " using Pointing::feed_angle=" + degrees + " deg\n";
    short_summary += " fa=" + degrees;

    feed_rotation = pointing->get_feed_angle();
  }
  else if (receiver)
  {
    string degrees = tostring(receiver->get_tracking_angle().getDegrees());
    summary += " using Receiver::tracking_angle=" + degrees + " deg\n";
    short_summary += " ta=" + degrees;

    feed_rotation = receiver->get_tracking_angle();
  }

  return get_los_rotation(feed_rotation);
}


Jones<double> Pulsar::ProjectionCorrection::get_antenna_projection () const
{
  if (must_correct_platform && should_correct_projection)
    return get_mount_projection ();
  else
    return get_antenna_rotation ();
}

Jones<double> Pulsar::ProjectionCorrection::get_antenna_rotation () const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::ProjectionCorrection::get_antenna_rotation" << endl;

  Angle antenna_rotation = 0.0;

  if (must_correct_platform && 
      should_correct_vertical && !should_correct_projection)
  {
    para.set_epoch( integration->get_epoch() );
 
    Directional* directional = para.get_directional();

    Angle para_pa = directional->get_parallactic_angle();
    std::string origin = directional->get_name();

    // check that the para_ang is equal

    if (pointing)
    {
      Angle pointing_pa = pointing->get_parallactic_angle();
      Angle feed_angle = pointing->get_feed_angle ();

      if (pointing_pa == 0.0 || !equal_pi( pointing_pa, para_pa ))
      {
        if (Archive::verbose)
        {
          cerr <<
            "Pulsar::ProjectionCorrection::get_antenna_rotation WARNING\n"
            "  Pointing parallactic angle="
               << pointing_pa.getDegrees() << " deg != \n"
            "  " << origin << " parallactic angle="
               << para_pa.getDegrees() << " deg" << endl;
        }

        if (Archive::verbose > 2)
        {
          MJD mjd = para.get_epoch();
          double lat = directional->get_observatory_latitude () * 180/M_PI;
          double lon = directional->get_observatory_longitude () * 180/M_PI;

          cerr << endl <<
            "  lat=" << lat << " deg, lon=" << lon << " deg, MJD=" << mjd;
        }
        
        if (trust_pointing_para_angle)
        {
          origin = "Pointing::";
          para_pa = pointing->get_parallactic_angle();
        }
        else
        {
          if (Archive::verbose)
            cerr << endl << "  correcting Pointing" << endl;
          const_kast(pointing)->set_parallactic_angle (para_pa);
        }
      }

      // correct position angle
      if (trust_pointing_feed_angle)
        const_kast(pointing)->set_position_angle (para_pa + feed_angle);
    }

    string degrees = tostring(para_pa.getDegrees());

    summary += " using " + origin + "::parallactic angle=" + degrees + " deg\n";
    short_summary += " pa=" + degrees;

    if (Archive::verbose > 2)
      cerr << "Pulsar::ProjectionCorrection::get_antenna_rotation"
              " adding vertical transformation\n  " << para.evaluate() << endl;
    
    antenna_rotation = para_pa;
  }

  return get_los_rotation(antenna_rotation);
}

Jones<double> Pulsar::ProjectionCorrection::get_mount_projection () const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::ProjectionCorrection::get__mount_projection" << endl;

  projection->set_epoch( integration->get_epoch() );

  Jones<double> J = projection->get_response();

  complex<double> det;
  Quaternion<double, Hermitian> herm;
  Quaternion<double, Unitary> unit;

  polar (det, herm, unit, J);

  summary += " using " + projection->get_name() + " projection "
          "\n\t J=" + tostring(J) + 
          "\n\t herm=" + tostring( herm.get_vector() ) +
          "\n\t unit=" + tostring( unit.get_vector() ) + "\n";

  short_summary += " " + projection->get_name() + " projection";
  
  return J;
}

std::string Pulsar::ProjectionCorrection::get_summary () const
{
  return summary;
}

std::string Pulsar::ProjectionCorrection::get_short_summary () const
{
  return short_summary;
}

//! Return the transformation matrix for the given epoch
Jones<double> Pulsar::ProjectionCorrection::operator () (unsigned isub) const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::ProjectionCorrection::operator" << endl;

  summary = "";
  short_summary = "";
  
  if (!required (isub))
    return 1.0;

  return get_feed_projection() * get_antenna_projection();
}
