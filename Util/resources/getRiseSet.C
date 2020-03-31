/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Site.h"

#include "Horizon.h"
#include "Pauli.h"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void usage ()
{
  cerr <<
    "getRiseSet - returns azimuth, zenith and parallactic angles\n"
    "\n"
    "options:\n"
    "\n"
    "  -c coord   coordinates of source in hh:mm:ss[.fs]<+|->dd:mm:ss[.fs] \n"
    "  -t site    telescope \n"
    "  -e min     minimum elevation \n"
    "\n"
       << endl;
}

int main (int argc, char* argv[]) 
{
  MJD mjd;
  sky_coord coord;

  Horizon horizon;

  Directional* directional = &horizon;

  // Parkes by default
  string telescope = "parkes";

  double min_elevation = 0.0;
  bool verbose = false;
 
  int c;
  while ((c = getopt(argc, argv, "hc:e:t:")) != -1)
  {
    switch (c)
    {
    case 'h':
      usage ();
      return 0;

    case 'c':
      coord.setHMSDMS (optarg);
      break;

    case 'e':
      min_elevation = atof (optarg);
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

  double lat, lon, rad;
  location->get_sph (lat, lon, rad);

  Angle latitude;
  Angle longitude;

  latitude.setRadians( lat );
  longitude.setRadians( lon );

  double rad2deg = 180.0/M_PI;
  double rad2hr = 12.0/M_PI;

  Mount* mount = directional;

  mount->set_source_coordinates( coord );
  mount->set_observatory_latitude( latitude.getRadians() );
  mount->set_observatory_longitude( longitude.getRadians() );

  double source_lst = coord.ra().getRadians();

  mount->set_local_sidereal_time( source_lst + M_PI );

  double elevation = 90 - (horizon.get_zenith() * rad2deg);

  if (elevation > min_elevation)
  {
    cout << "always never" << endl;
    return 0;
  }

  double min_lst = source_lst;
  double max_lst = source_lst + M_PI;
  double ten_seconds = 10.0 / (3600.0 * rad2hr);

  while ( (max_lst - min_lst) > ten_seconds )
  {
    double mid_lst = (min_lst + max_lst) * 0.5;
    mount->set_local_sidereal_time( mid_lst );

    double elevation = 90 - (horizon.get_zenith() * rad2deg);

    if (elevation > min_elevation)
      min_lst = mid_lst;
    else
      max_lst = mid_lst;
  }

  Angle lst_set = max_lst;
  Angle lst_rise = source_lst - (max_lst - source_lst);

  lst_set.setWrapPoint( 2*M_PI );
  lst_rise.setWrapPoint( 2*M_PI );

  cout << lst_rise.getHMS() << " " << lst_set.getHMS() << endl;

  if (verbose)
  {
    cerr << "\n*** Inputs:" << endl;
    cerr << "MJD: " << mjd.printdays(4) << endl;
    cerr << "Coordinates: " << coord.getHMSDMS() << endl;
    cerr << "  R.A.=" << coord.ra().getRadians() << endl;
    cerr << "  Dec.=" << coord.dec().getRadians() << endl;
    cerr << "Latitude: " << latitude.getDegrees() << " degrees "
         << latitude.getRadians() << " radians" << endl;
    cerr << "Longitude: " << longitude.getDegrees() << " degrees (East) "
         << longitude.getRadians() << " radians" << endl;

    cerr << "\n*** Outputs:\n"
      "GMST: " << mjd.GMST()*rad2hr << " hours "
         << mjd.GMST() << " radians\n"
      "LST: " << mount->get_local_sidereal_time()*rad2hr << " hours "
         << mount->get_local_sidereal_time() << " radians\n"
      "HA: " << mount->get_hour_angle()*rad2hr << " hours "
         << mount->get_hour_angle() << " radians "
       << mount->get_hour_angle()*rad2deg << " deg" <<endl;

    cerr <<
      "Azimuth: " << horizon.get_azimuth() * rad2deg << " degrees "
        << horizon.get_azimuth() << " radians\n"
      "Zenith: " << horizon.get_zenith() * rad2deg << " degrees  "
         << horizon.get_zenith() << " radians\n"
      "Altitude: " << 90 - (horizon.get_zenith() * rad2deg) << " degrees  "
         << M_PI/2 - horizon.get_zenith() << " radians\n";
  }

  return 0;
}


