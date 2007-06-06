/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"
#include "coord.h"

#include <iostream>

using namespace std;

void usage ()
{
  cout <<
    "getHorizon - returns azimuth zenith and parallactic angles\n"
    "USAGE:\n"
    "\n"
    "getHorizon -m MJD -c hh:mm:ss[.fs]<+|->dd:mm:ss[.fs] [-t isite]\n"
    "\n"
       << endl;
}

int main (int argc, char* argv[]) 
{
  MJD mjd;
  sky_coord coord;

  // Parkes by default
  char tempo_isite = '7';

  int c;
  while ((c = getopt(argc, argv, "hc:m:t:")) != -1) {

    switch (c)  {

    case 'h':
      usage ();
      return 0;

    case 'c':
      coord.setHMSDMS (optarg);
      break;

    case 'm':
      mjd.Construct (optarg);
      break;

    case 't':
      tempo_isite = optarg[0];
      break;

    }
  }

  float lat, lon, el;
  if (telescope_coords (tempo_isite, &lat, &lon, &el) < 0) {
    cerr << "telescope_coords failed for isite='" << tempo_isite << "'\n";
    return -1;
  }

  Angle latitude;
  Angle longitude;

  latitude.setDegrees( lat );
  longitude.setDegrees( lon );

  cerr << "Inputs:" << endl;
  cerr << "MJD: " << mjd.printdays(4) << endl;
  cerr << "Coordinates: " << coord.getHMSDMS() << endl;
  cerr << "Latitude: " << lat << " degrees" << endl;
  cerr << "Longitude: " << lon << " degrees" << endl;

  Horizon horizon;

  horizon.set_source_coordinates( coord );
  horizon.set_epoch( mjd );
  horizon.set_observatory_latitude( latitude.getRadians() );
  horizon.set_observatory_longitude( longitude.getRadians() );
  
  double rad2deg = 180.0/M_PI;

  cerr << "\nOutputs:\n"
    "LST: " << horizon.get_local_sidereal_time()*12.0/M_PI << " hours \n"
    "Azimuth: " << horizon.get_azimuth() * rad2deg << " degrees \n"
    "Zenith: " << horizon.get_zenith() * rad2deg << " degrees \n"
    "Parallactic: " << horizon.get_parallactic_angle() * rad2deg << " degrees"
       << endl;

  return 0;
}


