/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CelestialProjection.h"
#include "MountProjection.h"

using namespace std;

//! Default constructor
Calibration::CelestialProjection::CelestialProjection ()
{
  // the rotation angle is not a free parameter
}

//! Set the projection antenna
void Calibration::CelestialProjection::set_projection (MountProjection* d)
{
  projection = d;
  //if (projection)
  //   set_phi( projection->get_vertical() );
}

//! Get the projection antenna
MountProjection* Calibration::CelestialProjection::get_projection ()
{
  return projection;
}

//! Get the projection antenna
const MountProjection* Calibration::CelestialProjection::get_projection () const
{
  return projection;
}

void Calibration::CelestialProjection::set_epoch (const MJD& epoch)
{
  if (verbose)
    cerr << "Calibration::CelestialProjection::set_epoch " << epoch << endl;

  projection->set_epoch (epoch);
  // set_phi( projection->get_vertical() );
}

MJD Calibration::CelestialProjection::get_epoch () const
{
  return projection->get_epoch ();
}

//! Set the hour angle in radians
void Calibration::CelestialProjection::set_hour_angle (double radians)
{
  if (verbose)
    cerr << "Calibration::CelestialProjection::set_hour_angle " << radians << endl;

  projection->set_hour_angle (radians);

  // set_phi( projection->get_vertical() );
}

//! Get the hour angle in radians
double Calibration::CelestialProjection::get_hour_angle () const
{
  return projection->get_hour_angle ();
}

void Calibration::CelestialProjection::calculate (Jones<double>& result, std::vector<Jones<double>>* grad)
{
  if (!projection)
    throw Error (InvalidState, "Calibration::CelestialProjection::calculate",
     "no projection antenna has been set");

  result = projection->get_response();

  if (grad)
    grad->clear();
}