//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Quaternion.h,v $
   $Revision: 1.10 $
   $Date: 2003/02/26 10:05:01 $
   $Author: straten $ */

#ifndef __Quaternion_H
#define __Quaternion_H

#include <complex>
#include "psr_cpp.h"

//! Quaternion algebra is isomorphic with either Hermitian or Unitary matrices
enum Basis { Hermitian, Unitary };

//! Quaternion
template<typename T, Basis B = Unitary> class Quaternion {
  
public:
  T s0,s1,s2,s3;

  //! Null constructor
  Quaternion () { s0=s1=s2=s3=0; }

  //! Construct from T
  Quaternion (T s0_, T s1_, T s2_, T s3_)
    { s0=s0_; s1=s1_; s2=s2_; s3=s3_; }

  //! Construct from another Quaternion<U> instance
  template<typename U> Quaternion (const Quaternion<U, B>& s)
    { operator=(s); }

  //! Set this instance equal to another Quaternion<U> instance
  template<typename U> Quaternion& operator = (const Quaternion<U, B>& s)
    { s0=T(s.s0); s1=T(s.s1);
      s2=T(s.s2); s3=T(s.s3); return *this; }

  //! Vector addition
  Quaternion& operator += (const Quaternion& s)
    { s0+=s.s0; s1+=s.s1; s2+=s.s2; s3+=s.s3; return *this; }

  //! Vector subtraction
  Quaternion& operator -= (const Quaternion& s)
    { s0-=s.s0; s1-=s.s1; s2-=s.s2; s3-=s.s3; return *this; }

  //! Quaternion multiplication
  Quaternion& operator *= (const Quaternion& s)
    { *this = *this * s; return *this; }

  //! Scalar multiplication
  Quaternion& operator *= (T a)
    { s0*=a; s1*=a; s2*=a; s3*=a; return *this; }

  //! Scalar division
  Quaternion& operator /= (T a)
    { T d(1.0); d/=a; s0*=d; s1*=d; s2*=d; s3*=d; return *this; }

  //! Equality
  bool operator == (const Quaternion& b) const
    { return  s0==b.s0 && s1==b.s1 && s2==b.s2 && s3==b.s3; }

  //! Equality
  bool operator == (T scalar) const
    { return  s0==scalar && s1==0 && s2==0 && s3==0; }

  //! Inequality
  bool operator != (const Quaternion& b) const
    { return ! operator==(b); }


  //! Vector addition
  template<typename U>
  const friend Quaternion operator + (Quaternion a, const Quaternion<U,B>& b)
    { a+=b; return a; }

  //! Vector subtraction
  template<typename U> 
  const friend Quaternion operator - (Quaternion a, const Quaternion<U,B>& b)
    { a-=b; return a; }

  //! Scalar multiplication
  const friend Quaternion operator * (Quaternion a, T c)
    { a*=c; return a; }

  //! Scalar multiplication
  const friend Quaternion operator * (T c, Quaternion a)
    { a*=c; return a; }

  //! Scalar division
  const friend Quaternion operator / (Quaternion a, T c)
    { a/=c; return a; }

  //! complex multiplication
  template <typename U>
  const friend Quaternion operator * (const complex<U>& c, Quaternion a)
    { a*=c; return a; }

  //! complex multiplication
  template <typename U>
  const friend Quaternion operator * (Quaternion a, const complex<U>& c)
    { a*=c; return a; }

  //! complex division
  template <typename U>
  const friend Quaternion operator / (Quaternion a, const complex<U>& c)
    { a/=c; return a; }

  //! Negation
  const friend Quaternion operator - (Quaternion s)
    { s.s0=-s.s0; s.s1=-s.s1; s.s2=-s.s2; s.s3=-s.s3; return s; }

  //! Alternative access to elements
  T& operator [] (int n)
    { T* val = &s0; return val[n]; }
  
  //! Alternative access to elements 
  T operator [] (int n) const
    { return *(&s0+n); }

  //! Identity (should be const, wait for gcc version 3)
  static const Quaternion& identity();

  //! Dimension of data
  unsigned size () { return 4; }

};

//! The identity Quaternion
template<typename T, Basis B>
const Quaternion<T,B>& Quaternion<T,B>::identity ()
{
  static Quaternion I (1,0,0,0);
  return I;
}

//! Quick multiplication of a complex number by i
template<typename T>
complex<T> ci (const complex<T>& c)
{
  return complex<T> (-c.imag(), c.real());
}

//! Quick multiplication of a real number by i
template<typename T>
complex<T> ci (const T& real)
{
  return complex<T> (0, real);
}

//! Return the conjugate of a real number
template<typename T>
T conj (const T& x) { return x; };

//! Multiplication of two Biquaternions in the Hermitian basis
template<typename T>
const Quaternion<complex<T>, Hermitian>
operator * (const Quaternion<complex<T>,Hermitian>& a,
	    const Quaternion<complex<T>,Hermitian>& b)
{
  return Quaternion<complex<T>, Hermitian>
    ( a.s0*b.s0 + a.s1*b.s1 + a.s2*b.s2 + a.s3*b.s3 ,
      a.s0*b.s1 + a.s1*b.s0 + ci(a.s2*b.s3) - ci(a.s3*b.s2) ,
      a.s0*b.s2 - ci(a.s1*b.s3) + a.s2*b.s0 + ci(a.s3*b.s1) ,
      a.s0*b.s3 + ci(a.s1*b.s2) - ci(a.s2*b.s1) + a.s3*b.s0 );
}


//! Multiplication of two Quaternions in the Unitary basis
template<typename T>
const Quaternion<T, Unitary> operator * (const Quaternion<T,Unitary>& a,
					 const Quaternion<T,Unitary>& b)
{
  return Quaternion<T, Unitary>
    (a.s0*b.s0 - a.s1*b.s1 - a.s2*b.s2 - a.s3*b.s3,
     a.s0*b.s1 + a.s1*b.s0 - a.s2*b.s3 + a.s3*b.s2,
     a.s0*b.s2 + a.s1*b.s3 + a.s2*b.s0 - a.s3*b.s1,
     a.s0*b.s3 - a.s1*b.s2 + a.s2*b.s1 + a.s3*b.s0);
}


//! Returns the real component of a Biquaternion
template<typename T, Basis B>
Quaternion<T,B> real (const Quaternion<complex<T>,B>& j)
{
  return Quaternion<T,B>
    (j.s0.real(), j.s1.real(), j.s2.real(), j.s3.real());
}

//! Returns the imag component of a Biquaternion
template<typename T, Basis B>
Quaternion<T,B> imag (const Quaternion<complex<T>,B>& j)
{
  return Quaternion<T,B>
    (j.s0.imag(), j.s1.imag(), j.s2.imag(), j.s3.imag());
}

//! Returns the complex conjugate of a Hermitian Biquaternion
template<typename T>
Quaternion<T,Hermitian> conj (const Quaternion<T,Hermitian>& j)
{
  return Quaternion<T,Hermitian>
    (conj(j.s0), conj(j.s1), conj(j.s2), -conj(j.s3));
}

//! Returns the complex conjugate of a Unitary Quaternion
template<typename T>
Quaternion<T,Unitary> conj (const Quaternion<T,Unitary>& j)
{ 
  return Quaternion<T,Unitary>
    (conj(j.s0), -conj(j.s1), -conj(j.s2), conj(j.s3));
}


//! Returns the Hermitian transpose of a Hermitian Quaternion
template<typename T>
Quaternion<T, Hermitian> herm (const Quaternion<T,Hermitian>& j)
{
  return Quaternion<T,Hermitian>
    (conj(j.s0), conj(j.s1), conj(j.s2), conj(j.s3));
}


//! Returns the Hermitian transpose of a Unitary Quaternion
template<typename T>
Quaternion<T, Unitary> herm (const Quaternion<T,Unitary>& j)
{
  return Quaternion<T,Unitary>
    (conj(j.s0), -conj(j.s1), -conj(j.s2), -conj(j.s3));
}


//! Returns the inverse of Quaternion, j
template<typename T, Basis B>
Quaternion<T, B> inv (const Quaternion<T,B>& j) 
{
  T d (-1.0); d/=det(j);
  return Quaternion<T,B> (-d*j.s0, d*j.s1, d*j.s2, d*j.s3);
}


//! Returns the determinant of a Hermitian Quaternion
template<typename T>
T det (const Quaternion<T,Hermitian>& j)
{ return j.s0*j.s0 - j.s1*j.s1 - j.s2*j.s2 - j.s3*j.s3; }

//! Returns the determinant of a Unitary Quaternion
template<typename T>
T det (const Quaternion<T,Unitary>& j)
{ return j.s0*j.s0 + j.s1*j.s1 + j.s2*j.s2 + j.s3*j.s3; }




//! Returns the trace of Quaternion, j
template<typename T, Basis B>
T trace (const Quaternion<T,B>& j)
{ 
  return 2.0 * j.s0;
}


// Return the positive definite square root of a Hermitian Quaternion
template<typename T>
const Quaternion<T, Hermitian> sqrt (const Quaternion<T, Hermitian>& h)
{
  T mod = h.s1*h.s1 + h.s2*h.s2 + h.s3*h.s3;
  T det = sqrt (h.s0*h.s0 - mod);
  T scale = sqrt (0.5 * (h.s0 - det) / mod);

  return Quaternion<T, Hermitian>
    (sqrt (0.5 * (h.s0 + det)), h.s1 * scale, h.s2 * scale, h.s3 * scale);
}


//! Returns the square of the Frobenius norm of a Biquaternion
template<typename T, Basis B>
T norm (const Quaternion<complex<T>,B>& j)
{ 
  return 2.0 * (norm(j.s0) + norm(j.s1) + norm(j.s2) + norm(j.s3));
}

//! Returns the square of the Frobenius norm of a quaternion
template<typename T, Basis B>
T norm (const Quaternion<T,B>& j)
{ 
  return 2.0 * (j.s0*j.s0 + j.s1*j.s1 + j.s2*j.s2 + j.s3*j.s3);
}

template<typename T, Basis B>
T fabs (const Quaternion<T,B>& j)
{ 
  return sqrt (norm(j));
}

//! Useful for quickly printing the components
template<typename T>
ostream& operator<< (ostream& ostr, const Quaternion<T,Hermitian>& j)
{
  return ostr << "[h:" << j.s0 <<","<< j.s1 <<","<< j.s2 <<","<< j.s3 << "]";
}

//! Useful for quickly printing the components
template<typename T>
ostream& operator<< (ostream& ostr, const Quaternion<T,Unitary>& j)
{
  return ostr << "[u:" << j.s0 <<","<< j.s1 <<","<< j.s2 <<","<< j.s3 << "]";
}

#endif  /* not __Quaternion_H defined */

