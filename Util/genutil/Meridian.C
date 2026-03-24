/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Meridian.h"

//! Get the x angle in radians
double Meridian::get_x () const
{
  build ();
  return x;
}

//! Get the y angle in radians
double Meridian::get_y () const
{
  build ();
  return y;
}

//! Get the name of the mount
std::string Meridian::get_name () const
{
  return "Meridian";
}

//! Get the receptor basis in the reference frame of the observatory
Matrix<3,3,double> Meridian::get_basis (const Vector<3,double>& from) const
{
  /*
    angle in meridianal plane, positive toward North
  */
  x = - atan2 (from[0], from[2]);

  /*
    radius projected into meridianal plane
  */
  double r = sqrt (from[2]*from[2] + from[0]*from[0]);

  /*
    angle out of meridianal plane, positive toward East
  */
  y = - atan (from[1] / r);

  /*
    receptor basis in the frame of the observatory

    start with dish pointing toward zenith, and define a right-handed
    coordinate system with 

    x-axis toward North, y-axis toward East, z-axis toward Earth

    The Matrix returned by the rotation function defined in 
    epsic/src/util/Matrix.h rotates a vector about an axis by an angle
    as defined by the right-hand rule.  Therefore, to perform a basis
    transformation, rotate by negative angle.

    R1: rotate about East by x (tip basis to the North)
    then
    R2: rotate about North (prime) by -y (tip basis to the East)
  */

  Matrix<3,3,double> R1 = rotation (Vector<3,double>::basis(1), x);
  Matrix<3,3,double> R2 = rotation (Vector<3,double>::basis(0), -y);

  return R2 * R1;
}
