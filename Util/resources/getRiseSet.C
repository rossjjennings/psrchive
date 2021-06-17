/**************************************************************************
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
    "getRiseSet - returns rise and set time of source in LST\n"
    "\n"
    "options:\n"
    "\n"
    "  -c coord   coordinates of source in hh:mm:ss[.fs]<+|->dd:mm:ss[.fs] \n"
    "  -t site    telescope \n"
    "  -e min     minimum elevation \n"
    "  -l lst     current LST (or start LST) \n"
    "  -L lst     end LST \n"
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

  double min_elevation = 30.3;
  bool verbose = false;
  bool printspan = false;

  Angle start_lst;
  bool start_lst_specified = false;

  Angle end_lst;
  bool end_lst_specified = false;

  int c;
  while ((c = getopt(argc, argv, "hc:e:l:L:st:")) != -1)
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

    case 's':
      printspan = true;
      break;

    case 't':
      telescope = optarg;
      break;

    case 'l':
      start_lst.setHMS(optarg);
      start_lst_specified = true;
      break;

    case 'L':
      end_lst.setHMS(optarg);
      end_lst_specified = true;
      break;

    case 'v':
      verbose = true;
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
  double one_second = 1.0 / (3600.0 * rad2hr);

  while ( (max_lst - min_lst) > one_second )
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

  double span = 2 * (max_lst - source_lst);

  if (!start_lst_specified)
  {
    cout << lst_rise.getHMS(0) << " " << lst_set.getHMS(0);

    if (printspan)
      cout << " " << span * rad2hr;

    cout << endl;
  }
  else
  {
    double x0 = cos(start_lst.getRadians());
    double y0 = sin(start_lst.getRadians());

    double x1 = cos(source_lst);
    double y1 = sin(source_lst);

    double angle = atan2 (x1*y0-x0*y1, x0*x1+y0*y1);

    if (fabs(angle) < span*0.5)
    {
      if (end_lst_specified)
        cout << "UP - sets " << (span*.5 - angle) * rad2hr 
             << " hrs after start of session" << endl;
      else
        cout << "UP - sets in " << (span*.5 - angle) * rad2hr << " hrs" << endl;
    }
    else
    {
      double rises = (span*-.5 - angle);
      if (rises < 0)
        rises += 2*M_PI;

      double duration = 0;
 
      if (end_lst_specified)
      {
        double x2 = cos(end_lst.getRadians());
        double y2 = sin(end_lst.getRadians());

        duration = atan2 (x0*y2-x2*y0, x2*x0+y2*y0);

        if (rises > duration)
          cout << "DOWN" << endl;
        else
          cout << "SET - rises " << (duration-rises)*rad2hr 
               << " hrs before end of session" << endl;

        return 0;
      }

      
      cout << "SET - rises in " << rises*rad2hr << " hrs" << endl;

    }
  }

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


