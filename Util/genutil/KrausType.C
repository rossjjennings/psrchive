/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "KrausType.h"

#include <iostream>
using namespace std;

//! Get the name of the mount
std::string KrausType::get_name () const
{
  return "Kraus-type";
}

#if KRAUS_TYPE_USE_SPHERICAL_TRIGONOMETRY

/*! sin(PA) = sin(dec) * sin(-HA) */
double KrausType::get_vertical () const
{
  double HA = get_hour_angle();

  static bool print = true;
  if (print)
    cerr << "KrausType::get_vertical using spherical trigonometry" << endl;
  print = false;

  return asin( sin(declination) * sin(-HA) );
}

void KrausType::build () const
{
  Mount::build ();
}

#endif

Matrix<3,3,double> reflection (const Vector<3,double>& normal)
{
  auto P = outer(normal,normal);
  P *= 2.0;

  Matrix<3,3,double> identity (1.0);
  return identity - P;
}

//! Get the receptor basis in the reference frame of the observatory
Matrix<3,3,double> KrausType::get_basis (const Vector<3,double>& from) const
{
  /*
    elevation angle in meridianal plane
  */
  double elevation = atan2 (from[2], from[0]);

  // cerr << "KrausType::get_basis elevation=" << elevation*180.0/M_PI << " deg" << endl;

  /*
    length of from source vector projected into meridianal plane
  */
  double r = sqrt (from[2]*from[2] + from[0]*from[0]);

  /*
    angle out of meridianal plane, positive toward East
  */
  double y = - atan (from[1] / r);

  /*
    receptor basis in the frame of the observatory, where
    x = North, 
    y = East, and 
    z = -Zenith

    start with feed pointing toward North, and define a left-handed
    coordinate system with 

    x-axis toward Zenith, 
    y-axis toward East, 
    z-axis toward South

    The reference frames starts out left-handed because
    1. physically, the feed was already reflected off the secondary mirror
    2. it will be returned to a right-handed system after reflection off the primary mirror

  */
  Vector<3,double> Zenith (0,0,-1);
  Vector<3,double> East (0,1,0);
  Vector<3,double> South (-1,0,0);

  Matrix<3,3,double> feed;
  feed[0] = Zenith;
  feed[1] = East;
  feed[2] = South;

  /*
    The Matrix returned by the rotation function defined in 
    epsic/src/util/Matrix.h rotates a vector about an axis by an angle
    as defined by the right-hand rule.  Therefore, to perform a basis
    transformation, rotate by negative angle.

    R1: rotate about Zenith by y (turn basis toward the East)
    then
    R2: relect basis in mirror
  */

  auto R1 = rotation (Vector<3,double>::basis(0), -y);

  // normal to mirror in reference frame of observatory
  auto normal = cos(0.5*elevation)*South + sin(0.5*elevation)*Zenith;
  // normal to mirror in rotated feed frame
  auto nprime = R1 * feed * normal;
  // reflection matrix in rotated feed frame
  auto R2 = reflection(nprime);

  // cerr << "KrausType::get_basis" << endl;
  return R2 * R1 * feed;
}
