//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Stokes.h,v $
   $Revision: 1.5 $
   $Date: 2003/02/27 14:49:10 $
   $Author: straten $ */

#ifndef __Stokes_H
#define __Stokes_H

#include "Pauli.h"
#include "random.h"
#include "Error.h"

template <typename T>
class Stokes : public Quaternion<T, Hermitian>
{
 public:
  
  Stokes (T a=0, T b=0, T c=0, T d=0) : Quaternion<T,Hermitian> (a,b,c,d) {}

  template <typename U>
    Stokes (const Quaternion<U,Hermitian>& q) : Quaternion<T,Hermitian> (q) {}
  
  template <typename U>
    Stokes (const Quaternion<complex<U>,Hermitian>& q)
    { operator = (q); }

  template <typename U>
    Stokes (const Jones<U>& j)
    { operator = (j); }

  template <typename U>
    Stokes& operator = (const Quaternion<U,Hermitian>& q)
  { Quaternion<T,Hermitian>::operator = (q); return *this; }
  
  template <typename U>
    Stokes& operator = (const Quaternion<complex<U>,Hermitian>& q)
  { Quaternion<T,Hermitian>::operator = (real(q));
    Quaternion<T,Hermitian> imaginary (imag(q));
    if (norm(imaginary) > 1e-5 * norm(*this))
      throw Error (InvalidParam, 
		   "Stokes::operator = Quaternion<complex<U>,Hermitian>",
		   "non-zero imaginary component");
    return *this; }

  template <typename U>
    Stokes& operator = (const Jones<U>& j)
  { return operator = ( convert(j) ); }

  T abs_vect () const { return sqrt (s1*s1 + s2*s2 + s3*s3); }

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

  if (det(val) < 0.0)
    throw Error (InvalidState, "random_value (Stokes)",
		 "det=%f<0", det(val));
}

template <class T, class U>
void random_vector (Stokes<T>& val, U scale)
{
  random_value (val, scale);
}


#endif
