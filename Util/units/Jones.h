//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Jones.h,v $
   $Revision: 1.6 $
   $Date: 2003/02/24 18:04:53 $
   $Author: straten $ */

#ifndef __Jones_H
#define __Jones_H

#include <complex>
#include <stdlib.h>

#include "psr_cpp.h"


//! Jones matrices are 2x2 matrices with complex elements
template<typename T> class Jones {
  
public:
  complex<T> j11,j12,j21,j22;

  //! Null constructor
  Jones () {  }

  //! Construct from complex<T>
  Jones (complex<T> j11_, complex<T> j12_,
	 complex<T> j21_, complex<T> j22_)
    { j11=j11_; j12=j12_; j21=j21_; j22=j22_; }

  //! Construct from another Jones<T> matrix
  Jones (const Jones& s)
    { operator=(s); }

  //! Construct from another Jones<U> matrix
  template<typename U> Jones (const Jones<U>& s)
    { operator=(s); }

  //! Set this instance equal to another Jones<T> instance
  Jones& operator = (const Jones& s)
    { j11=s.j11; j12=s.j12; j21=s.j21; j22=s.j22; return *this; }

  //! Set this instance equal to a scalar
  Jones& operator = (T scalar)
    { j11=scalar; j12=0; j21=0; j22=scalar; return *this; }

  //! Set this instance equal to another Jones<U> instance
  template<typename U> Jones& operator = (const Jones<U>& s)
    { j11=complex<T>(s.j11); j12=complex<T>(s.j12);
      j21=complex<T>(s.j21); j22=complex<T>(s.j22); return *this; }

  //! Add another Jones<T> instance to this one
  Jones& operator += (const Jones& s)
    { j11+=s.j11; j12+=s.j12; j21+=s.j21; j22+=s.j22; return *this; }

  //! Subract another Jones<T> instance from this one
  Jones& operator -= (const Jones& s)
    { j11-=s.j11; j12-=s.j12; j21-=s.j21; j22-=s.j22; return *this; }

  //! Multiply another Jones<T> instance into this one (this=this*j)
  Jones& operator *= (const Jones& j);

  //! Multiply this instance by complex<U>
  template<typename U> Jones& operator *= (const complex<U>& au)
    { complex<T>a(au); j11*=a; j12*=a; j21*=a; j22*=a; return *this; }

  //! Divide this instance by complex<U>
  template<typename U> Jones& operator /= (const complex<U>& au)
    { complex<T>a(1.0); a/=au; j11*=a; j12*=a; j21*=a; j22*=a; return *this; }

  //! Multiply this instance by T
  Jones& operator *= (T a)
    { j11*=a; j12*=a; j21*=a; j22*=a; return *this; }

  //! Divide this instance by T
  Jones& operator /= (T a)
    { T d=1.0/a; j11*=d; j12*=d; j21*=d; j22*=d; return *this; }

  //! Equality
  bool operator == (const Jones& b) const
  { return 
      j11 == b.j11  &&  j12 == b.j12 && 
      j21 == b.j21  &&  j22 == b.j22;
  }

  //! Equality
  bool operator == (T scalar) const
  { return 
      j11 == scalar  &&  j12 == 0 && 
      j21 == 0  &&  j22 == scalar;
  }

  //! Inequality
  bool operator != (const Jones& b) const
  { return ! Jones::operator==(b); }


  //! Binary addition
  template<typename U>
  const friend Jones operator + (Jones a, const Jones<U>& b)
    { a+=b; return a; }

  //! Binary subtraction
  template<typename U> 
  const friend Jones operator - (Jones a, const Jones<U>& b)
    { a-=b; return a; }

  //! Binary multiplication of two Jones matrices
  template<typename U>
  const friend Jones operator * (Jones a, const Jones<U>& b)
    { a*=b; return a; }

  //! Binary multiplication of Jones<T> and complex<U>
  template<typename U> const friend Jones operator * (Jones a, complex<U> c)
    { a*=c; return a; }

  //! Binary multiplication of complex<U> and Jones<T>
  template<typename U> const friend Jones operator * (complex<U> c, Jones a)
    { a*=c; return a; }

  //! Binary multiplication of Jones<T> and T
  const friend Jones operator * (Jones a, T c)
    { a*=c; return a; }

  //! Binary multiplication of T and Jones<T>
  const friend Jones operator * (T c, Jones a)
    { a*=c; return a; }

  //! Binary division of Jones by any type
  template<typename U> const friend Jones operator / (Jones a, U c)
    { a/=c; return a; }

  //! Negation operator returns negative of instance
  const friend Jones operator - (Jones s)
    { s.j11=-s.j11; s.j12=-s.j12; s.j21=-s.j21; s.j22=-s.j22; return s; }

  //! Returns reference to the value of the matrix at j(ir,ic)
  complex<T>& j (int ir, int ic)
  { complex<T>* val = &j11; return val[ir*2+ic]; }
  
  //! Returns const reference to the value of the matrix at j(ir,ic)
  const complex<T>& j (int ir, int ic) const
    { const complex<T>* val = &j11; return val[ir*2+ic]; }

  //! The identity matrix
  static const Jones& identity();

  //! Return a random Jones matrix
  static const Jones random (T scale);

};

//! The identity matrix
template<typename T>
const Jones<T>& Jones<T>::identity ()
{
  static Jones<T> I (1,0,
 	             0,1);
  return I;
}


inline double randouble (double scale)
{
  return ( double(rand()) - 0.5*double(RAND_MAX) ) * 2*scale / RAND_MAX;
}

template <class T>
const Jones<T> Jones<T>::random (T scale)
{
  return Jones<T> (complex<T>(randouble(scale), randouble(scale)),
		   complex<T>(randouble(scale), randouble(scale)),
		   complex<T>(randouble(scale), randouble(scale)),
		   complex<T>(randouble(scale), randouble(scale)));
}

//! Multiply another Jones<T> instance into this one (this=this*j)
template<typename T>
Jones<T>& Jones<T>::operator *= (const Jones<T>& j)
{
  complex<T> temp;
  temp = j11 * j.j11 + j12 * j.j21;
  j12  = j11 * j.j12 + j12 * j.j22; j11=temp;
  temp = j21 * j.j11 + j22 * j.j21;
  j22  = j21 * j.j12 + j22 * j.j22; j21=temp;
  return *this; 
}

//! Returns the inverse
template<typename T>
Jones<T> inv (const Jones<T>& j)
{
  complex<T> d(1.0); d/=det(j);
  return Jones<T>(d*j.j22, -d*j.j12,
		  -d*j.j21, d*j.j11);
}

//! Returns the complex conjugate
template<typename T>
Jones<T> conj (const Jones<T>& j)
{
  return Jones<T>(std::conj(j.j11), std::conj(j.j12),
		  std::conj(j.j21), std::conj(j.j22));
}

//! Returns the Hermitian transpose (transpose of complex conjugate)
template<typename T> 
Jones<T> herm (const Jones<T>& j)
{
  return Jones<T>(std::conj(j.j11), std::conj(j.j21),
		  std::conj(j.j12), std::conj(j.j22));
}

//! Returns the determinant
template<typename T>
complex<T> det (const Jones<T>& j) { return j.j11*j.j22 - j.j12*j.j21; }

//! Returns the trace
template<typename T>
complex<T> trace (const Jones<T>& j) { return j.j11 + j.j22; }

//! Returns the variance (square of the Frobenius norm)
template<typename T>
T norm (const Jones<T>& j)
{ return
    norm(j.j11) + norm(j.j12) + 
    norm(j.j21) + norm(j.j22);
}

template<typename T>
T fabs (const Jones<T>& j)
{ 
  return sqrt (norm(j));
}

//! Useful for quickly printing the values of matrix elements
template<typename T>
ostream& operator<< (ostream& ostr, const Jones<T>& j)
{
  return ostr << "[" << j.j11 << j.j12 << j.j21 << j.j22 << "]";
}

#endif  /* not __Jones_H defined */

