/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Site.h"

#include "Horizon.h"
#include "Meridian.h"

#include <iostream>
#include <unistd.h>

using namespace std;

void usage ()
{
  cout <<
    "getHorizon - returns azimuth, zenith and parallactic angles\n"
    "USAGE:\n"
    "\n"
    "getHorizon -m MJD -c hh:mm:ss[.fs]<+|->dd:mm:ss[.fs] [-t telescope] [-M]"
    "\n"
    "  -M to switch to Meridian (X-Y) coordinates"
    "\n"
       << endl;
}

int main (int argc, char* argv[]) 
{
  MJD mjd;
  sky_coord coord;

  Horizon horizon;
  Meridian meridian;

  Directional* directional = &horizon;

  // Parkes by default
  string telescope = "parkes";

  int c;
  while ((c = getopt(argc, argv, "hc:Mm:t:")) != -1)
  {
    switch (c)
    {
    case 'h':
      usage ();
      return 0;

    case 'c':
      coord.setHMSDMS (optarg);
      break;

    case 'M':
      directional = &meridian;
      break;

    case 'm':
      mjd.Construct (optarg);
      break;

    case 't':
      telescope = optarg;
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

  if (mjd == MJD::zero)
  {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    cerr << "\nUsing current date/time: " << asctime(&date);
    mjd = MJD (date);
  }

  double lat, lon, rad;
  location->get_sph (lat, lon, rad);

  Angle latitude;
  Angle longitude;

  latitude.setRadians( lat );
  longitude.setRadians( lon );

  cout << "\n*** Inputs:" << endl;
  cout << "MJD: " << mjd.printdays(4) << endl;
  cout << "Coordinates: " << coord.getHMSDMS() << endl;
  cout << "Latitude: " << latitude.getDegrees() << " degrees" << endl;
  cout << "Longitude: " << longitude.getDegrees() << " degrees" << endl;

  directional->set_source_coordinates( coord );
  directional->set_epoch( mjd );
  directional->set_observatory_latitude( latitude.getRadians() );
  directional->set_observatory_longitude( longitude.getRadians() );
  
  double rad2deg = 180.0/M_PI;

  cout << "\n*** Outputs:\n"
    "LST: " << directional->get_local_sidereal_time()*12.0/M_PI << " hours \n";

  if (directional == &horizon)
  {
    cout <<
      "Azimuth: " << horizon.get_azimuth() * rad2deg << " degrees \n"
      "Zenith: " << horizon.get_zenith() * rad2deg << " degrees \n";
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


