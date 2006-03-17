/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Horizon.h"
#include "sky_coord.h"

#include <slalib.h>

Horizon::Horizon ()
{
  declination = right_ascension = 0;
  longitude = latitude = 0;
  built = false;
}

void Horizon::set_source_coordinates (const sky_coord& pos)
{
  declination = pos.dec().getRadians();
  right_ascension = pos.ra().getRadians();

  built = false;
}

void Horizon::set_observatory_latitude (double lat)
{
  latitude = lat;
  built = false;
}

double Horizon::get_observatory_latitude () const
{
  return latitude;
}

void Horizon::set_observatory_longitude (double longi)
{
  longitude = longi;
  built = false;
}

double Horizon::get_observatory_longitude () const
{
  return longitude;
}

void Horizon::set_epoch (const MJD& _epoch)
{
  if (epoch == _epoch)
    return;

  epoch = _epoch;
  built = false;
}

MJD Horizon::get_epoch () const
{
  return epoch;
}

//! Get the hour_angle in radians
double Horizon::get_hour_angle () const
{
  build ();
  return hour_angle;
}

//! Get the parallactic angle in radians
double Horizon::get_parallactic_angle () const
{
  build ();
  return parallactic_angle;
}

//! Get the azimuth angle in radians
double Horizon::get_azimuth () const
{
  build ();
  return azimuth;
}

//! Get the elevation angle in radians
double Horizon::get_elevation () const
{
  build ();
  return elevation;
}

//! Get the zenith angle in radians
double Horizon::get_zenith () const
{
  build ();
  return M_PI - elevation;
}

void Horizon::do_build ()
{
  // MJD::LST receives longitude in degrees and returns LST in hours
  double lst = epoch.LST (longitude * 180/M_PI);

  // compute hour angle in radians
  hour_angle = lst * M_PI/12.0 - right_ascension;

  double ignore;
  slaAltaz (hour_angle, declination, latitude, 
	    &azimuth,                &ignore, &ignore,
	    &elevation,              &ignore, &ignore,
	    &parallactic_angle,      &ignore, &ignore);

  built = true;
}
