//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Vector.h,v $
   $Revision: 1.1 $
   $Date: 2003/03/03 09:56:43 $
   $Author: straten $ */

#ifndef __Vector_H
#define __Vector_H

#include <iostream>
#include "psr_cpp.h"

//! Vector
template <typename T, unsigned N> 
class Vector {
  
public:
  T x[N];

  //! Null constructor
  Vector () 
  { for (unsigned i=0; i<N; i++) x[i] = 0; }

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
  Vector& operator *= (T a)
    { for (unsigned i=0; i<N; i++) x[i] *= a; return *this; }

  //! Scalar division
  Vector& operator /= (T a)
    { T d = 1.0; d/=a;  return operator *= (d); }

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
  T operator [] (unsigned n) const
    { return x[n]; }

  //! Dimension of data
  unsigned size () { return N; }

};

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

