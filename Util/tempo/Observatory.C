
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Observatory.h"
#include <math.h>

Tempo::Observatory::Observatory ()
{
  code = 0;
}

char Tempo::Observatory::get_code () const
{
  return code;
}

void Tempo::Observatory::set_code (char c)
{
  code = c;
}

std::string Tempo::Observatory::get_itoa_code () const
{
  return itoa_code;
}

void Tempo::Observatory::set_itoa_code (const std::string& c)
{
  if (c.length() != 2)
    throw Error (InvalidParam, "Tempo::Observatory::set_itoa_code",
		 "code=%s length != 2", c.c_str());

  itoa_code = c;
}

std::string Tempo::Observatory::get_name () const
{
  return name;
}

void Tempo::Observatory::set_name (const std::string& n)
{
  name = n;
}

Tempo::ObservatoryITRF::ObservatoryITRF (double _x, double _y, double _z)
{
  x = _x; y = _y; z = _z;
}


void Tempo::ObservatoryITRF::get_xyz (double& _x, double& _y, double& _z)
{
  _x = x; _y = y; _z = z;
}

//! Get the latitude and longitude in radians
void Tempo::ObservatoryITRF::get_latlonel (double& lat,
					   double& lon,
					   double& el)
{
  double radius = x*x + y*y + z*z;
  lat = asin (z/radius);
  lon = atan2 (-y, x);

  // no time
  el = 0.0;
}


//! Default constructor
Tempo::ObservatoryWGS84::ObservatoryWGS84 (double _lat,
					   double _lon,
					   double _el)
{
  lat = _lat; lon = _lon; el = _el;
};

//! Get the geocentric XYZ coordinates in metres
void Tempo::ObservatoryWGS84::get_xyz (double& x, double& y, double& z)
{
  throw Error (InvalidState, "Tempo::ObservatoryWGS84::get_xyz",
	       "not implemented");
}

//! Get the latitude and longitude in radians
void Tempo::ObservatoryWGS84::get_latlonel (double& _lat,
					    double& _lon,
					    double& _el)
{
  _lat = lat; _lon = lon; _el = el;
};

