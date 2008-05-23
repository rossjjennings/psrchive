
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Observatory.h"
#include "tempo++.h"

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

static string itoa_code (const string& telescope_name);

const Tempo::Observatory*
Tempo::observatory (const string& telescope_name)
{
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

class aliases
{
public:
  aliases (const string& code, const string& alias)
  {
    itoa_code = code;
    aka.push_back (alias);
  }

  bool match (const string& name)
  {
    for (unsigned i=0; i<aka.size(); i++)
      if (strcasecmp (aka[i].c_str(), name.c_str()) == 0)
	return true;
    return false;
  }

  string itoa_code;
  vector<string> aka;
};

static int init = 0;

static vector<aliases> itoa_aliases;

static int default_aliases ();

string itoa_code (const string& telescope_name)
{
  if (!init)
    default_aliases ();

  for (unsigned i=0; i<itoa_aliases.size(); i++)
    if (itoa_aliases[i].match( telescope_name ))
      return itoa_aliases[i].itoa_code;

  cerr << "itoa_code no alias found for " << telescope_name << endl;

  return string();
}

void add_alias (const string& itoa_code, const string& alias)
{
  for (unsigned i=0; i<itoa_aliases.size(); i++)
    if (itoa_aliases[i].itoa_code == itoa_code)
    {
      itoa_aliases[i].aka.push_back (alias);
      return;
    }

  itoa_aliases.push_back ( aliases( itoa_code, alias ) );
}

int default_aliases ()
{
  add_alias ("GB", "gbt");
  add_alias ("GB", "green bank");
  add_alias ("GB", "greenbank");

  add_alias ("NA", "atca");
  add_alias ("NA", "narrabri");

  add_alias ("AO", "arecibo");

  add_alias ("HO", "hobart");

  add_alias ("NS", "urumqi");

  add_alias ("TD", "tid");
  add_alias ("TD", "tidbinbilla");
  add_alias ("TD", "DSS43");
  add_alias ("TD", "DSS 43");

  add_alias ("PK", "pks");
  add_alias ("PK", "parkes");

  add_alias ("JB", "jodrell");
  add_alias ("JB", "jodrell bank");

  add_alias ("VL", "vla");

  add_alias ("BO", "northern cross");

  add_alias ("MO", "most");

  add_alias ("NC", "nancay");

  add_alias ("EF", "effelsberg");

  add_alias ("WT", "wsrt");
  add_alias ("WT", "westerbork");

  add_alias ("GM", "gmrt");

  return 1;
}


