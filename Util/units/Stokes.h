//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Stokes.h,v $
   $Revision: 1.1 $
   $Date: 2003/02/05 13:20:40 $
   $Author: straten $ */

#ifndef __Stokes_H
#define __Stokes_H

#include "Pauli.h"

template <typename T>
class Stokes : public Quaternion<T, Hermitian>
{
 public:
  
  Stokes (T a=0, T b=0, T c=0, T d=0) : Quaternion<T,Hermitian> (a,b,c,d) {}

  template <typename U>
    Stokes (const Quaternion<U,Hermitian>& q) : Quaternion<T,Hermitian> (q) {}
  
  template <typename U>
    Stokes (const Quaternion<complex<U>,Hermitian>& q) : 
    Quaternion<T,Hermitian> (real(q))
    { /* check that imag(q) is small */ }

  template <typename U>
    Stokes (const Jones<U>& j) : Quaternion<T,Hermitian> ( real(convert(j)) )
    { /* check that imag(q) is small */ }

  template <typename U>
    Stokes& operator = (const Quaternion<U,Hermitian>& q)
  { Quaternion<T,Hermitian>::operator = (q); return *this; }
  
  template <typename U>
    Stokes& operator = (const Quaternion<complex<U>,Hermitian>& q)
  { Quaternion<T,Hermitian>::operator = (real(q)); return *this; }

  template <typename U>
    Stokes& operator = (const Jones<U>& j)
  { Quaternion<T,Hermitian>::operator = ( real(convert(j)) ); return *this; }

};

#endif
