//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Pauli.h,v $
   $Revision: 1.2 $
   $Date: 2003/02/05 23:27:59 $
   $Author: straten $ */

#ifndef __Pauli_H
#define __Pauli_H

#include "Jones.h"
#include "Quaternion.h"

// convert Hermitian Quaternion to Jones matrix
template<typename T>
const Jones<T> convert (const Quaternion<T,Hermitian>& q)
{
  return Jones<T> (q.s0+q.s1, q.s2-ci(q.s3),
		   q.s2+ci(q.s3), q.s0-q.s1);
}

// convert Unitary Quaternion to Jones matrix
template<typename T>
const Jones<T> convert (const Quaternion<T,Unitary>& q)
{
  return Jones<T> (q.s0+ci(q.s1), q.s3+ci(q.s2),
		   -q.s3+ci(q.s2), q.s0-ci(q.s1));
}

// convert Jones matrix to Hermitian Biquaternion
template<typename T>
const Quaternion<complex<T>, Hermitian> convert (const Jones<T>& j)
{
  return Quaternion<complex<T>, Hermitian>
    ( 0.5 * (j.j11 + j.j22),
      0.5 * (j.j11 - j.j22),
      0.5 * (j.j12 + j.j21),
      0.5 * ci (j.j12 - j.j21) );
}


// convert Jones matrix to Hermitian and Unitary Quaternion
template<typename T>
void polar (complex<T>& d, Quaternion<T, Hermitian>& h,
	    Quaternion<T, Unitary>& u, Jones<T> j)
{
  // make j unimodular
  d = sqrt (det(j));
  j /= d;

  // calculate the square of h
  h = real (convert (j*herm(j)));

  T hpsq = h.s1*h.s1 + h.s2*h.s2 + h.s3*h.s3;
  T hdet = sqrt (h.s0*h.s0 - hpsq);
  T scale = sqrt (0.5 * (h.s0 - hdet)) / sqrt(hpsq);

  h.s0 = sqrt (0.5 * (h.s0 + hdet));
  h.s1 *= scale;
  h.s2 *= scale;
  h.s3 *= scale;

  // take the hermitian component out of j
  j = inv(convert(h)) * j;

  u.s0 = 0.5 * (j.j11 + j.j22).real();
  u.s1 = 0.5 * (j.j11 - j.j22).imag();
  u.s2 = 0.5 * (j.j12 + j.j21).imag();
  u.s3 = 0.5 * (j.j12 - j.j21).real();
}


// multiply a Jones matrix by a Quaternion
template<typename T, typename U, Basis B>
const Jones<T> operator * (const Jones<T>& j, const Quaternion<U,B>& q)
{
  return j * convert(q);
}

// multiply a Jones matrix by a Quaternion
template<typename T, typename U, Basis B>
const Jones<T> operator * (const Quaternion<T,B>& q, const Jones<U>& j)
{
  return convert(q) * j;
}

#endif
