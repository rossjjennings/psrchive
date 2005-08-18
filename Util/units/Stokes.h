//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Stokes.h,v $
   $Revision: 1.15 $
   $Date: 2005/08/18 12:10:42 $
   $Author: straten $ */

#ifndef __Stokes_H
#define __Stokes_H

#include "Quaternion.h"
#include "random.h"
#include "Error.h"

template <typename T>
class Stokes : public Quaternion<T, Hermitian>
{
 public:
  
  Stokes (T a=0.0, T b=0.0, T c=0.0, T d=0.0) 
    : Quaternion<T,Hermitian> (a,b,c,d) {}

  //! Construct from a scalar and vector
  template<typename U>
    Stokes (T s, const Vector<3,U>& v) : Quaternion<T,Hermitian> (s, v) {}

  template<typename U>
    Stokes (const Stokes<U>& s)
  {
    Quaternion<T,Hermitian>::s0 = s.Quaternion<U,Hermitian>::s0;
    Quaternion<T,Hermitian>::s1 = s.Quaternion<U,Hermitian>::s1;
    Quaternion<T,Hermitian>::s2 = s.Quaternion<U,Hermitian>::s2;
    Quaternion<T,Hermitian>::s3 = s.Quaternion<U,Hermitian>::s3;
  }

  T abs_vect () const { return sqrt (Quaternion<T,Hermitian>::s1*Quaternion<T,Hermitian>::s1 + Quaternion<T,Hermitian>::s2*Quaternion<T,Hermitian>::s2 + Quaternion<T,Hermitian>::s3*Quaternion<T,Hermitian>::s3); }

};

// useful method for generating random source polarization
template <class T, class U>
void random_value (Stokes<T>& val, U scale, float max_polarization = 1.0)
{
  // total intensity is always equal to scale
  val.s0 = scale;

  // generate a random fractional polarization, from 0 to 1
  T fraction_polarized;
  random_value (fraction_polarized, 0.5);
  fraction_polarized += 0.5;
  fraction_polarized *= max_polarization;

  unsigned i=0;
  
  for (i=1; i<4; i++)
    random_value (val[i], scale);

  T modp = val.abs_vect();
  scale *= fraction_polarized / modp;

  for (i=1; i<4; i++)
    val[i] *= scale;

  if (det(val) < -1e-10)
    throw Error (InvalidState, "random_value (Stokes)",
		 "det=%f<0", det(val));
}

template <class T, class U>
void random_vector (Stokes<T>& val, U scale)
{
  random_value (val, scale);
}

#endif
