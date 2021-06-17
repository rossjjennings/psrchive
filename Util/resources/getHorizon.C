/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Site.h"

#include "Horizon.h"
#include "Meridian.h"
#include "Fixed.h"
#include "Pauli.h"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void usage ()
{
  cout <<
    "getHorizon - returns azimuth, zenith and parallactic angles\n"
    "\n"
    "options:\n"
    "\n"
    "  -c coord   coordinates of source in hh:mm:ss[.fs]<+|->dd:mm:ss[.fs] \n"
    "  -d hours   duration of observation in hours \n"
    "  -m mjd     time (MJD) of observation (start time if -d is used) \n"
    "  -l lst     time (LST) of observation (start time if -d is used) \n"
    "  -M         use Meridian (X-Y) coordinates \n"
    "  -F         simulate fixed receptors \n"
    "  -o deg:deg compute delta-parallactic angle for offset antenna \n"
    "  -t site    telescope \n"
    "\n"
       << endl;
}

int main (int argc, char* argv[]) 
{
  MJD mjd;
  sky_coord coord;

  bool use_lst = false;
  double lst = 0.0;

  Horizon horizon;
  Meridian meridian;
  Fixed* fixed = 0;

  Directional* directional = &horizon;

  // Parkes by default
  string telescope = "parkes";

  double duration = 0.0;

  string offset;

  int c;
  while ((c = getopt(argc, argv, "hc:d:FMl:m:o:t:")) != -1)
  {
    switch (c)
    {
    case 'h':
      usage ();
      return 0;

    case 'c':
      coord.setHMSDMS (optarg);
      break;

    case 'd':
      duration = atof (optarg);
      break;

    case 'F':
      fixed = new Fixed;
      break;

    case 'M':
      directional = &meridian;
      break;

    case 'l':
      lst = atof (optarg);
      use_lst = true;
      break;

    case 'm':
      mjd.Construct (optarg);
      break;

    case 't':
      telescope = optarg;
      break;

    case 'o':
      offset = optarg;
      break;

    }
  }

  const Pulsar::Site* location = 0;

  try
  {
    location = Pulsar::Site::location (telescope);
  }
  catch (Error& error)
  {
    cerr << "Tempo::observatory failed for telescope='" << telescope << "'\n";
    return -1;
  }

  if (mjd == MJD::zero && !use_lst)
  {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    cerr << "\nUsing current date/time: " << asctime(&date);
    mjd = MJD (date);
    cerr << "MJD=" << mjd.printdays(5) << endl;
  }

  double lat, lon, rad;
  location->get_sph (lat, lon, rad);

  Angle latitude;
  Angle longitude;

  latitude.setRadians( lat );
  longitude.setRadians( lon );

  double rad2deg = 180.0/M_PI;
  double rad2hr = 12.0/M_PI;

  Mount* mount = directional;
  if (fixed)
    mount = fixed;

  mount->set_source_coordinates( coord );
  if (use_lst)
    mount->set_local_sidereal_time( lst/rad2hr );
  else
    mount->set_epoch( mjd );

  mount->set_observatory_latitude( latitude.getRadians() );
  mount->set_observatory_longitude( longitude.getRadians() );

#if 0
  Mount* mount2 = 0;
  if (offset)
  {
    mount2 = mount->clone();
  
  }
#endif

  if (duration)
  {
    unsigned nsteps = 100;
    for (unsigned i=0; i<nsteps; i++)
    {
      double hours = (duration * i) / nsteps;
      if (use_lst)
	mount->set_local_sidereal_time( (lst + hours)/rad2hr );
      else
      {
	MJD cur = mjd + hours * 3600.0;
	mount->set_epoch( cur );
      }

      if (fixed)
      {
	complex<double> det;
	Quaternion<double, Hermitian> herm;
	Quaternion<double, Unitary> unit;

	Jones<double> J = fixed->get_response();

	polar (det, herm, unit, J);

	Stokes<double> input (2, 1, 0, 0.8);
	Stokes<double> output = transform (input, J);

	// J = conj(J);
	// output = transform (output, inv(J));

	cerr << hours << " " << det 
	     << " " << herm.get_vector()
	     << " " << unit.get_vector() 
	     << " " << output.get_vector() << endl;

	cout << hours
	     << " " << output.get_vector()[0] 
	     << " " << output.get_vector()[1] 
	     << " " << output.get_vector()[2] << endl;
      }
      else
	cout << hours << " " << mount->get_vertical() * rad2deg
	     << " " << mount->get_local_sidereal_time() * rad2hr << endl;
    }
    return 0;
  }

  cout << "\n*** Inputs:" << endl;
  cout << "MJD: " << mjd.printdays(4) << endl;
  cout << "Coordinates: " << coord.getHMSDMS() << endl;
  cout << "  R.A.=" << coord.ra().getRadians() << endl;
  cout << "  Dec.=" << coord.dec().getRadians() << endl;
  cout << "Latitude: " << latitude.getDegrees() << " degrees "
       << latitude.getRadians() << " radians" << endl;
  cout << "Longitude: " << longitude.getDegrees() << " degrees (East) "
       << longitude.getRadians() << " radians" << endl;

  cout << "\n*** Outputs:\n"
    "GMST: " << mjd.GMST()*rad2hr << " hours "
       << mjd.GMST() << " radians\n"
    "LST: " << mount->get_local_sidereal_time()*rad2hr << " hours "
       << mount->get_local_sidereal_time() << " radians\n"
    "HA: " << mount->get_hour_angle()*rad2hr << " hours "
       << mount->get_hour_angle() << " radians "
       << mount->get_hour_angle()*rad2deg << " deg" <<endl;

  if (fixed)
  {
    Jones<double> response = fixed->get_response();

    cerr << response << endl;
  }

  else if (directional == &horizon)
  {
    cout <<
      "Azimuth: " << horizon.get_azimuth() * rad2deg << " degrees "
       << horizon.get_azimuth() << " radians\n"
      "Zenith: " << horizon.get_zenith() * rad2deg << " degrees  "
       << horizon.get_zenith() << " radians\n"
      "Altitude: " << 90 - (horizon.get_zenith() * rad2deg) << " degrees  "
       << M_PI/2 - horizon.get_zenith() << " radians\n";
  }
  else
  {
    cout <<
      "X: " << meridian.get_x() * rad2deg << " degrees \n"
      "Y: " << meridian.get_y() * rad2deg << " degrees \n";
  }

  cout <<
    "Parallactic: " << directional->get_vertical() * rad2deg << " degrees \n"
       << endl;

  return 0;
}


