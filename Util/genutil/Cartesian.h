//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Cartesian.h,v $
   $Revision: 1.3 $
   $Date: 2000/03/07 03:23:45 $
   $Author: straten $ */

#ifndef __CARTESIAN_H
#define __CARTESIAN_H

#include <iostream>
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

  double& operator[] (char dimension);
  // const double& operator [] (int dimension) const;

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

  friend Cartesian min (const Cartesian& cart1, const Cartesian& cart2);
  friend Cartesian max (const Cartesian& cart1, const Cartesian& cart2);
      
  double modSquared () const;
  double mod () const;
  static Angle angularSeparation (const Cartesian& c1, const Cartesian& c2);
};

ostream& operator<< (ostream& ostr, const Cartesian&);

#endif // __CARTESIAN_H
