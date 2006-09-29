/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Parallactic.h"
#include "Pauli.h"
#include "sky_coord.h"

#include <limits.h>
#include <slalib.h>

using namespace std;

//! Default constructor
Calibration::Parallactic::Parallactic ()
  : Rotation1 (Pauli::basis.get_basis_vector(2))
{
  // the rotation angle is not a free parameter
  set_infit (0, false);

  declination = 0;
  right_ascension = 0;

  longitude = latitude = 0;

  current_hour_angle = 0.0;
}

//! Set the coordinates of the source
void Calibration::Parallactic::set_source_coordinates (const sky_coord& pos)
{
  declination = pos.dec().getRadians();
  right_ascension = pos.ra().getRadians();

  reset ();
}

//! Set the coordinates of the observatory
void
Calibration::Parallactic::set_observatory_coordinates (double lat, double lon)
{
  latitude = lat;
  longitude = lon;

  reset ();
}

void Calibration::Parallactic::set_epoch (const MJD& epoch)
{
  if (verbose)
    cerr << "Calibration::Parallactic::set_epoch " << epoch << endl;

  if (epoch == current_epoch)
    return;

  // MJD::LST receives longitude in degrees and returns LST in hours
  double lst = epoch.LST(longitude);

  double hour_angle = lst * M_PI/12.0 - right_ascension;

  set_hour_angle (hour_angle);

  current_epoch = epoch;
}

MJD Calibration::Parallactic::get_epoch () const
{
  return current_epoch;
}

//! Set the hour angle in radians
void Calibration::Parallactic::set_hour_angle (double hour_angle)
{
  if (verbose)
    cerr << "Calibration::Parallactic::set_hour_angle " << hour_angle << endl;

  if (hour_angle == current_hour_angle)
    return;

  // declination stored in radians
  double dec = declination;
  // latitude stored in degrees
  double lat = latitude * M_PI/180.0;
  // hour angle specified in radians
  double ha  = hour_angle;

  double azimuth, elevation, para;
  double ignore;

  slaAltaz (ha, dec, lat, 
	    &azimuth,   &ignore, &ignore,
	    &elevation, &ignore, &ignore,
	    &para,      &ignore, &ignore);

  // Rotate the basis about the line of sight by para
  set_phi (para);
  current_hour_angle = hour_angle;
}

double Calibration::Parallactic::get_hour_angle () const
{
  return current_hour_angle;
}

void Calibration::Parallactic::set_parallactic_angle (double para)
{
  set_phi (para);
  current_hour_angle = 99;
}

double Calibration::Parallactic::get_parallactic_angle () const
{
  return get_phi().get_value();
}

void Calibration::Parallactic::reset ()
{
  float hour_angle = current_hour_angle;
  current_hour_angle -= 1.0;
  set_hour_angle (hour_angle);
}
