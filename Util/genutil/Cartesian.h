/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Cartesian.h,v $
   $Revision: 1.2 $
   $Date: 2000/02/29 07:40:52 $
   $Author: straten $ */

#ifndef __CARTESIAN_H
#define __CARTESIAN_H

#include "angle.h"

class Cartesian
{
 public:
  double x, y, z;

  Cartesian ();
  Cartesian (double ux, double uy, double uz);
  Cartesian (const AnglePair& spherical);
  Cartesian (const Cartesian &);

  Cartesian & operator =  (const Cartesian &);
  Cartesian & operator += (const Cartesian &);
  Cartesian & operator -= (const Cartesian &);
  Cartesian & operator *= (double linear);
  Cartesian & operator /= (double linear);

  const friend Cartesian operator + (const Cartesian &, const Cartesian &);
  const friend Cartesian operator - (const Cartesian &, const Cartesian &);
  const friend Cartesian operator * (const Cartesian &, double);  
  const friend Cartesian operator / (const Cartesian &, double);

  // cross product
  const friend Cartesian operator % (const Cartesian &, const Cartesian &);
  // scalar product
  friend double operator * (const Cartesian &, const Cartesian &);

  friend int operator == (const Cartesian &, const Cartesian &);
  friend int operator != (const Cartesian &, const Cartesian &);

  double modSquared () const;
  double mod () const;
  static Angle angularSeparation (const Cartesian& c1, const Cartesian& c2);
};

#endif // __CARTESIAN_H
