//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Quaternion.h,v $
   $Revision: 1.2 $
   $Date: 2003/01/27 16:17:43 $
   $Author: straten $ */

#ifndef __Quaternion_H
#define __Quaternion_H

#include "psr_cpp.h"

//! Quaternion algebra is isomorphic with either Hermitian or Unitary matrices
enum Basis { Hermitian, Unitary };

//! Quaternion
template<typename T, Basis B = Unitary> class Quaternion {
  
public:
  T s0,s1,s2,s3;

  //! Null constructor
  Quaternion () {  }

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
    { *this = *this * j; return *this; }

  //! Scalar multiplication
  Quaternion& operator *= (T a)
    { s0*=a; s1*=a; s2*=a; s3*=a; return *this; }

  //! Scalar division
  Quaternion& operator /= (T a)
    { T d=1.0/a; s0*=d; s1*=d; s2*=d; s3*=d; return *this; }

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

  //! Negation
  const friend Quaternion operator - (Quaternion s)
    { s.s0=-s.s0; s.s1=-s.s1; s.s2=-s.s2; s.s3=-s.s3; return s; }

  //! Alternative access to elements
  T& operator [] (int n)
    { T* val = &s0; return val[n]; }
  
  //! Alternative access to elements 
  T operator [] (int n) const
    { return *(&s0+n); }

  //! Returns the inverse
  Quaternion inv () const { return conj()/norm(*this); }
  
  //! Returns the complex conjugate
  Quaternion conj () const { return Quaternion (s0, -s1, -s2, -s3); };
  
  //! Returns the Hermitian transpose (transpose of complex conjugate)
  Quaternion dag () const { throw "undefined"; }
  
  //! Identity (should be const, wait for gcc version 3)
  static Quaternion identity;

};

//! The identity Quaternion
template<typename T, Basis B>
Quaternion<T,B> Quaternion<T,B>::identity (1,0,0,0);

//! Multiplication of two Quaternions in the Hermitian basis
template<typename T>
const Quaternion<T, Hermitian> operator * (const Quaternion<T,Hermitian>& a,
					   const Quaternion<T,Hermitian>& b)
{
  return Quaternion<T, Hermitian>
    (a.s0*b.s0 + a.s1*b.s1 + a.s2*b.s2 + a.s3*b.s3,
     a.s0*b.s1 + a.s1*b.s0 - a.s2*b.s3 + a.s3*b.s2,
     a.s0*b.s2 + a.s1*b.s3 + a.s2*b.s0 - a.s3*b.s1,
     a.s0*b.s3 - a.s1*b.s2 + a.s2*b.s1 + a.s3*b.s0);
}


//! Multiplication of two Quaternions in the Unitary basis
template<typename T>
const Quaternion<T, Unitary> operator * (const Quaternion<T,Unitary>& a,
					 const Quaternion<T,Unitary>& b)
{
  return Quaternion<T, Unitary>
    (a.s0*b.s0 - a.s1*b.s1 - a.s2*b.s2 - a.s3*b.s3,
     a.s0*b.s1 + a.s1*b.s0 + a.s2*b.s3 - a.s3*b.s2,
     a.s0*b.s2 - a.s1*b.s3 + a.s2*b.s0 + a.s3*b.s1,
     a.s0*b.s3 + a.s1*b.s2 - a.s2*b.s1 + a.s3*b.s0);
}

#if 0

//! Returns the determinant
template<typename T, Basis B>
T det (const Quaternion<T,B>& j) { return j.j11*j.j22 - j.j12*j.j21; }

//! Returns the trace
template<typename T, Basis B>
complex<T> trace (const Quaternion<T,B>& j) { return j.j11 + j.j22; }

//! Returns the variance (square of the Frobenius norm)
template<typename T, Basis B>
T norm (const Quaternion<T,B>& j)
{ return
    norm(j.j11) + norm(j.j12) + 
    norm(j.j21) + norm(j.j22); }

#endif


//! Equality
template<typename T, typename U, Basis B>
bool operator == (const Quaternion<T,B>& a, const Quaternion<U,B>& b)
{ return 
    a.s0==T(b.s0) && a.s1==T(b.s1) && 
    a.s2==T(b.s2) && a.s3==T(b.s3);
}

//! Inequality
template<typename T, typename U, Basis B>
bool operator != (const Quaternion<T,B>& a, const Quaternion<U,B>& b)
{ return !(a==b); }

//! Useful for quickly printing the components
template<typename T, Basis B>
ostream& operator<< (ostream& ostr, const Quaternion<T,B>& j)
{
  return ostr << "[" << j.s0 <<","<< j.s1 <<","<< j.s2 <<","<< j.s3 << "]";
}

#endif  /* not __Quaternion_H defined */

