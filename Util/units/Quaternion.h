//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Quaternion.h,v $
   $Revision: 1.1 $
   $Date: 2003/01/27 15:51:17 $
   $Author: straten $ */

#ifndef __HQuaternion_H
#define __HQuaternion_H

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

  //! Construct from another Quaternion<U> matrix
  template<typename U> Quaternion (const Quaternion<U, B>& s)
    { operator=(s); }

  //! Set this instance equal to another Quaternion<U> instance
  template<typename U> Quaternion& operator = (const Quaternion<U, B>& s)
    { s0=T(s.s0); s1=T(s.s1);
      s2=T(s.s2); s3=T(s.s3); return *this; }

  //! Add another Quaternion<T> instance to this one
  Quaternion& operator += (const Quaternion& s)
    { s0+=s.s0; s1+=s.s1; s2+=s.s2; s3+=s.s3; return *this; }

  //! Subract another Quaternion<T> instance from this one
  Quaternion& operator -= (const Quaternion& s)
    { s0-=s.s0; s1-=s.s1; s2-=s.s2; s3-=s.s3; return *this; }

  //! Multiply another Quaternion<T> instance into this one (this=this*j)
  Quaternion& operator *= (const Quaternion& j)
    { *this = *this * j; return *this; }

  //! Multiply this instance by T
  Quaternion& operator *= (T a)
    { s0*=a; s1*=a; s2*=a; s3*=a; return *this; }

  //! Divide this instance by T
  Quaternion& operator /= (T a)
    { T d=1.0/a; s0*=d; s1*=d; s2*=d; s3*=d; return *this; }

  //! Binary addition
  template<typename U>
  const friend Quaternion operator + (Quaternion a, const Quaternion<U>& b)
    { a+=b; return a; }

  //! Binary subtraction
  template<typename U> 
  const friend Quaternion operator - (Quaternion a, const Quaternion<U>& b)
    { a-=b; return a; }

  //! Binary multiplication of Quaternion<T> and T
  const friend Quaternion operator * (Quaternion a, T c)
    { a*=c; return a; }

  //! Binary multiplication of T and Quaternion<T>
  const friend Quaternion operator * (T c, Quaternion a)
    { a*=c; return a; }

  //! Binary division of Quaternion by T
  const friend Quaternion operator / (Quaternion a, T c)
    { a/=c; return a; }

  //! Negation operator returns negative of instance
  const friend Quaternion operator - (Quaternion s)
    { s.s0=-s.s0; s.s1=-s.s1; s.s2=-s.s2; s.s3=-s.s3; return s; }

  T& operator [] (int n)
    { T* val = &s0; return val[n]; }

  T operator [] (int n) const
    { return *(&s0+n); }

  //! The identity matrix (should be const, wait for gcc version 3)
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

