//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Pauli.h,v $
   $Revision: 1.6 $
   $Date: 2003/02/15 10:25:11 $
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
    ( T(0.5) * (j.j11 + j.j22),
      T(0.5) * (j.j11 - j.j22),
      T(0.5) * (j.j12 + j.j21),
      T(0.5) * ci (j.j12 - j.j21) );
}

// Return the positive definite root of a Hermitian Quaternion
template<typename T>
const Quaternion<T, Hermitian> sqrt (const Quaternion<T, Hermitian>& h)
{
  T mod = h.s1*h.s1 + h.s2*h.s2 + h.s3*h.s3;
  T det = sqrt (h.s0*h.s0 - mod);
  T scale = sqrt (0.5 * (h.s0 - det) / mod);

  return Quaternion<T, Hermitian>
    (sqrt (0.5 * (h.s0 + det)), h.s1 * scale, h.s2 * scale, h.s3 * scale);
}

// convert Jones matrix to Hermitian and Unitary Quaternion
template<typename T>
void polar (complex<T>& d, Quaternion<T, Hermitian>& h,
	    Quaternion<T, Unitary>& u, Jones<T> j)
{
  // make j unimodular
  d = sqrt (det(j));
  j /= d;

  // calculate the hermitian
  h = sqrt( real( convert( j*herm(j) ) ) );

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

// multiply Quaternions from different Basis
template<typename T, typename U, Basis A, Basis B>
const Jones<T> operator * (const Quaternion<T,A>& q, const Quaternion<U,B>& u)
{
  return convert(q) * convert(u);
}


#endif
