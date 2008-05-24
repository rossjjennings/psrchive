
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Observatory.h"
#include "tempo++.h"
#include "tempo_impl.h"

#include <math.h>

using namespace std;

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


void Tempo::ObservatoryITRF::get_xyz (double& _x, double& _y, double& _z) const
{
  _x = x; _y = y; _z = z;
}

//! Get the latitude and longitude in radians
void Tempo::ObservatoryITRF::get_latlonel (double& lat,
					   double& lon,
					   double& el) const
{
#ifdef _DEBUG
  cerr << "Tempo::ObservatoryITRF::get_latlonel"
          " x=" << x << " y=" << y << " z=" << z << endl;
#endif

  double radius = sqrt(x*x + y*y + z*z);
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
void Tempo::ObservatoryWGS84::get_xyz (double& x, double& y, double& z) const
{
  throw Error (InvalidState, "Tempo::ObservatoryWGS84::get_xyz",
	       "not implemented");
}

//! Get the latitude and longitude in radians
void Tempo::ObservatoryWGS84::get_latlonel (double& _lat,
					    double& _lon,
					    double& _el) const
{
  _lat = lat; _lon = lon; _el = el;
};


const Tempo::Observatory*
Tempo::observatory (const string& telescope_name)
{
  load_obsys ();

  // if the name is a single character, then it is likely a tempo site number
  if (telescope_name.length() == 1)
    for (unsigned i=0; i < antennae.size(); i++)
      if (antennae[i]->get_code() == telescope_name[0])
	return antennae[i];

  for (unsigned i=0; i < antennae.size(); i++)
    if (strcasestr( antennae[i]->get_name().c_str(), telescope_name.c_str() ))
      return antennae[i];

  string itoa = itoa_code (telescope_name);

  for (unsigned i=0; i < antennae.size(); i++)
    if (antennae[i]->get_itoa_code() == itoa)
      return antennae[i];

  throw Error (InvalidParam, "Tempo::observatory",
               "no antennae named '" + telescope_name + "'");
}

