//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Vector.h,v $
   $Revision: 1.9 $
   $Date: 2004/07/19 11:40:05 $
   $Author: straten $ */

#ifndef __Vector_H
#define __Vector_H

#include <iostream>
#include <complex>

#include "psr_cpp.h"

//! Vector
template <typename T, unsigned N> 
class Vector {
  
public:
  T x[N];

  //! Null constructor
  Vector () 
  { for (unsigned i=0; i<N; i++) x[i] = 0; }

  Vector (T x0, T x1, T x2)
  { x[0] = x0; x[1] = x1; x[2] = x2; }

  //! Construct from another Vector<U> instance
  template<typename U> Vector (const Vector<U, N>& s)
    { operator=(s); }

  //! Set this instance equal to another Vector<U> instance
  template<typename U> Vector& operator = (const Vector<U, N>& s)
    { for (unsigned i=0; i<N; i++) x[i] = T(s.x[i]); return *this; }

  //! Set this instance equal to a scalar
  Vector& operator = (const T& scalar)
    { x[0] = scalar; for (unsigned i=1; i<N; i++) x[i] = 0.0; return *this; }

  //! Vector addition
  Vector& operator += (const Vector& s)
    { for (unsigned i=0; i<N; i++) x[i] += s.x[i]; return *this; }

  //! Vector subtraction
  Vector& operator -= (const Vector& s)
    { for (unsigned i=0; i<N; i++) x[i] -= s.x[i]; return *this; }

  /*! Vector multiplication
    Vector& operator *= (const Vector& s)
    { *this = *this * s; return *this; }
  */

  //! Scalar multiplication
  template<typename U>
  Vector& operator *= (const U& a)
    { for (unsigned i=0; i<N; i++) x[i] *= a; return *this; }

  //! Scalar division
  template<typename U>
  Vector& operator /= (const U& a)
    { for (unsigned i=0; i<N; i++) x[i] /= a; return *this; }

  //! Equality
  bool operator == (const Vector& b) const
    { for(unsigned i=0; i<N; i++) if(x[i]!=b.x[i]) return false; return true; }

  //! Inequality
  bool operator != (const Vector& b) const
    { return ! operator==(b); }

  //! Vector addition
  template<typename U>
  const friend Vector operator + (Vector a, const Vector<U,N>& b)
    { a+=b; return a; }

  //! Vector subtraction
  template<typename U> 
  const friend Vector operator - (Vector a, const Vector<U,N>& b)
    { a-=b; return a; }

  //! Scalar multiplication
  const friend Vector operator * (Vector a, T c)
    { a*=c; return a; }

  //! Scalar multiplication
  const friend Vector operator * (T c, Vector a)
    { a*=c; return a; }

  //! Dot product
  const friend T operator * (const Vector& a, const Vector& b)
    { T r=0; for (unsigned i=0; i<N; i++) r += a[i]*b[i]; return r; }

  //! Scalar division
  const friend Vector operator / (Vector a, T c)
    { a/=c; return a; }

  //! Negation
  const friend Vector operator - (Vector s)
    { for (unsigned i=0; i<N; i++) s.x[i] = -s.x[i]; return s; }

  //! Access to elements
  T& operator [] (unsigned n)
    { return x[n]; }
  
  //! Alternative access to elements 
  const T operator [] (unsigned n) const
    { return x[n]; }

  //! Dimension of data
  unsigned size () const { return N; }

  //! Return the basis vector
  static const Vector basis (unsigned i)
    { Vector v;  v[i] = 1.0; return v; }

};

//! Cross product
template <typename T> 
const Vector<T,3> cross (const Vector<T,3>& a, const Vector<T,3>& b)
{
  Vector<T,3> result;
  unsigned j, k;
  for (unsigned i=0; i<3; i++) {
    j = (i+1)%3;  k = (i+2)%3;
    result[i] = a[j]*b[k] - a[k]*b[j];
  }

  return result;
}

//! squared "length"/"norm"
template <typename T, unsigned N>
T normsq(const Vector<complex<T>, N> &v)
{
  T sum = norm(v[0]);
  for (unsigned i=1; i < N; i++)
    sum += norm(v[i]);
  return sum;
}

//! simpler version for scalar types
template <typename T, unsigned N>
T normsq(const Vector<T, N> &v)
{
  T sum = v[0]*v[0];
  for (unsigned i=1; i < N; i++)
    sum += v[i]*v[i];
  return sum;
}

//! and the norm itself; note, won't work for complex
template <typename T, unsigned N>
T norm(const Vector<T, N> &v)
{
  return sqrt(normsq(v));
}

//! Useful for quickly printing the components
template<typename T, unsigned N>
ostream& operator<< (ostream& ostr, const Vector<T,N>& v)
{
  ostr << "(" << v[0];
  for (unsigned i=1; i<N; i++)
    ostr << "," << v[i];
  return ostr << ")";
}

#endif  /* not __Vector_H defined */

