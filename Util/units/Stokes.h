//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Stokes.h,v $
   $Revision: 1.2 $
   $Date: 2003/02/15 10:24:14 $
   $Author: straten $ */

#ifndef __Stokes_H
#define __Stokes_H

#include "Pauli.h"
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

};

#endif
