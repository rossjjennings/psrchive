/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"

using namespace std;

Horizon::Horizon (double az, double el)
{
  azimuth = az;
  elevation = el;

  minimum_azimuth = 0.0;
  maximum_azimuth = 2*M_PI;
  minimum_elevation = 0.0;
  maximum_elevation = M_PI/4;

  // 1 deg per second
  azimuth_velocity = elevation_velocity = 1.0 * M_PI/180.0;
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
  return 0.5 * M_PI - elevation;
}

//! Get the name of the mount
std::string Horizon::get_name () const
{
  return "Horizon";
}

//! Get the receptor basis in the reference frame of the observatory
Matrix<3,3,double> Horizon::get_basis (const Vector<3,double>& from) const
{
  /*
    angle toward source in horizontal plane.
    'from' points 180 degrees away from the source.
    this form also ensures that 0 < az < 360 degrees.
  */
  azimuth = M_PI + atan2 (from[1], from[0]);

  while (azimuth > maximum_azimuth)
    azimuth -= 2*M_PI;

  while (azimuth < minimum_azimuth)
    azimuth += 2*M_PI;

  /*
    radius projected into horizontal plane
  */
  double r = sqrt (from[1]*from[1] + from[0]*from[0]);

  /*
    elevation angle out of horizontal plane
  */
  elevation = atan (from[2] / r);

  double zenith = 0.5*M_PI - elevation;

  /*
    receptor basis in the frame of the observatory

    start with dish pointing toward zenith, and define a right-handed
    coordinate system with 

    x-axis toward North, y-axis toward East, z-axis toward Earth

    R1: rotate about zenith so that North is 180 degrees away from source,
    then
    R2: rotate about y-prime to tip dish down toward source by zenith angle

    The Matrix returned by the rotation function defined in 
    epsic/src/util/Matrix.h rotates a vector about an axis by an angle
    as defined by the right-hand rule.  Therefore, to perform a basis
    transformation, rotate by negative angle.
  */

  Matrix<3,3,double> R1 = rotation (Vector<3,double>::basis(2), M_PI - azimuth);
  Matrix<3,3,double> R2 = rotation (Vector<3,double>::basis(1), - zenith);

  return R2 * R1;
}

std::vector< std::pair<double,Mount*> >
Horizon::slew_times (const sky_coord& coords)
{
  Horizon next (*this);
  next.set_source_coordinates (coords);

  return slew_times( next.get_azimuth(), next.get_elevation() );
}

std::vector< std::pair<double,Mount*> >
Horizon::slew_times (double next_azimuth, double next_elevation)
{
#if _DEBUG
  cerr << "Horizon::slew_times current az=" << get_azimuth()*180/M_PI
       << " el=" << get_elevation()*180/M_PI << endl;

  cerr << "Horizon::slew_times next az=" << next_azimuth*180/M_PI
       << " el=" << next_elevation*180/M_PI << endl;
#endif

  double diff_elevation = fabs( get_elevation() - next_elevation );
  double time_elevation = diff_elevation / elevation_velocity;
  
  // consider every valid slew in azimuth
  while (next_azimuth > minimum_azimuth)
    next_azimuth -= 2*M_PI;

  // after the above loop, the azimuth will be less than the minimum
  next_azimuth += 2*M_PI;

  std::vector< std::pair<double,Mount*> > retval;

  while (next_azimuth < maximum_azimuth)
  {
    double diff_azimuth = fabs( get_azimuth() - next_azimuth );
    double time_azimuth = diff_azimuth / azimuth_velocity;

    double time = std::max (time_azimuth, time_elevation);

    Horizon* next = new Horizon (*this);
    next->azimuth = next_azimuth;

    retval.push_back( std::pair<double,Mount*>( time, next ) );

    next_azimuth += 2*M_PI;
  }
  
  return retval;
}

