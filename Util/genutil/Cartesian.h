//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Cartesian.h,v $
   $Revision: 1.4 $
   $Date: 2000/05/28 04:31:38 $
   $Author: straten $ */

#ifndef __CARTESIAN_H
#define __CARTESIAN_H

#include <iostream>
#include "angle.h"

class Cartesian
{
 public:
  double x, y, z;

  Cartesian (double ux = 0.0, double uy = 0.0, double uz = 0.0);
  Cartesian (const AnglePair& spherical);
  Cartesian (const Cartesian &);

  Cartesian & operator =  (const Cartesian &);
  Cartesian & operator += (const Cartesian &);
  Cartesian & operator -= (const Cartesian &);
  Cartesian & operator *= (double linear);
  Cartesian & operator /= (double linear);

  double& operator[] (char dimension);
  // const double& operator [] (int dimension) const;

  const friend Cartesian operator + (const Cartesian &, const Cartesian &);
  const friend Cartesian operator - (const Cartesian &, const Cartesian &);
  const friend Cartesian operator * (const Cartesian &, double);  
  const friend Cartesian operator / (const Cartesian &, double);

  // just does a piece-wise division (x/x, y/y, z/z)
  const friend Cartesian pdiv (const Cartesian &, const Cartesian &);
  // just does a piece-wise multiplication (x*x, y*y, z*z)
  const friend Cartesian pmult (const Cartesian &, const Cartesian &);

  // cross product
  const friend Cartesian operator % (const Cartesian &, const Cartesian &);
  // scalar product
  friend double operator * (const Cartesian &, const Cartesian &);

  friend int operator == (const Cartesian &, const Cartesian &);
  friend int operator != (const Cartesian &, const Cartesian &);

  friend Cartesian min (const Cartesian& cart1, const Cartesian& cart2);
  friend Cartesian max (const Cartesian& cart1, const Cartesian& cart2);
  friend void diagonalize (Cartesian& bottom_left, Cartesian& upper_right);

  double modSquared () const;
  double mod () const;
  static Angle angularSeparation (const Cartesian& c1, const Cartesian& c2);
};

ostream& operator<< (ostream& ostr, const Cartesian&);

#endif // __CARTESIAN_H
