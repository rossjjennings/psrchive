//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Matrix.h,v $
   $Revision: 1.5 $
   $Date: 2004/04/06 16:35:48 $
   $Author: straten $ */

#ifndef __Matrix_H
#define __Matrix_H

#include <iostream>

#include "Vector.h"
#include "Error.h"
#include "psr_cpp.h"

//! Matrix is a column vector of row vectors
template <typename T, unsigned Rows, unsigned Columns> 
class Matrix : public Vector< Vector<T, Columns>, Rows > {
  
public:

  //! Null constructor
  Matrix () { for (unsigned i=0; i<Rows; i++) x[i] = 0; }

  //! Construct from another Matrix<U> instance
  template<typename U> Matrix (const Matrix<U, Rows, Columns>& s)
  { operator=(s); }

  //! Set this instance equal to another Matrix<U> instance
  template<typename U> Matrix& operator = (const Matrix<U, Rows, Columns>& s)
  { for (unsigned i=0; i<Rows; i++) x[i] = s.x[i]; return *this; }
  
};

//! Vector multiplication
template<typename T, typename U, unsigned Rows, unsigned Columns>
const Vector<U,Rows> operator * (const Matrix<T,Rows,Columns>& m,
                                 const Vector<U,Columns>& b)
{
  Vector<U,Rows> r;
  for (unsigned i=0; i<Rows; i++)
    r[i] = Vector<U,Columns>(m[i]) * b;
  return r;
}

//! Matrix multiplication
template<typename T, unsigned R1, unsigned C1R2, unsigned C2>
const Matrix<T, R1, C2>
operator * (const Matrix<T, R1, C1R2>& a, const Matrix<T, C1R2, C2>& b)
{ 
  Matrix<T, R1, C2> r; 
  for (unsigned i=0; i<R1; i++)
    for (unsigned j=0; j<C2; j++)
      for (unsigned k=0; k<C1R2; k++)
	r[i][j] += a[i][k]*b[k][j];
  return r;
}

template <typename T, typename U, unsigned Rows, unsigned C1, unsigned C2>
void GaussJordan (Matrix<T,Rows,C1>& a, Matrix<U,Rows,C2>& b)
{
  Vector<unsigned, Rows> indxc;
  Vector<unsigned, Rows> indxr;
  Vector<unsigned, Rows> ipiv;

  unsigned irow = 0;
  unsigned icol = 0;

  unsigned i, j, k;
  for (i=0; i<Rows; i++) {

    double big = 0.0;

    //cerr << "0" << endl;

    for (j=0; j<Rows; j++) {
      if (ipiv[j] != 1)
	for (k=0; k<Rows; k++) {
	  if (ipiv[k] == 0) {
	    if (fabs(a[j][k]) >= big) {
	      big=fabs(a[j][k]);
	      irow=j;
	      icol=k;
	    }
	  } 
	  else if (ipiv[k] > 1) 
	    throw Error (InvalidParam, "GaussJordan", "Singular Matrix-1");
	}
    }

    //cerr << "1" << endl;

    ipiv[icol]++;
    
    if (irow != icol) {
      for (j=0; j<Rows; j++)
	swap (a[irow][j], a[icol][j]);
      for (j=0; j<C2; j++)
	swap (b[irow][j], b[icol][j]);
    }

    //cerr << "2" << endl;

    indxr[i]=irow;
    indxc[i]=icol;

    if (a[icol][icol] == 0.0)
      throw Error (InvalidParam, "GaussJordan", "Singular Matrix-2");

    //cerr << "3" << endl;

    T pivinv = 1.0/a[icol][icol];
    a[icol][icol]=1.0;

    for (j=0; j<Rows; j++)
      a[icol][j] *= pivinv;
    for (j=0; j<C2; j++)
      b[icol][j] *= pivinv;

    //cerr << "4" << endl;

    for (j=0; j<Rows; j++)
      if (j != icol) {
	T dum = a[j][icol];
	a[j][icol]=0.0;
	for (k=0; k<Rows; k++) 
	  a[j][k] -= a[icol][k]*dum;
	for (k=0; k<C2; k++)
	  b[j][k] -= b[icol][k]*dum;
      }

    //cerr << "5" << endl;
  }

  //cerr << "6" << endl;
    
  for (i=Rows; i>0; i--) {
    if (indxr[i-1] != indxc[i-1])
      for (j=0; j<Rows; j++)
	swap(a[j][indxr[i-1]],a[j][indxc[i-1]]);
  }
  
   //cerr << "7" << endl;
 
}

template <typename T, unsigned RC>
void identity (Matrix<T, RC, RC>& m)
{
  for (unsigned i=0; i<RC; i++)
    for (unsigned j=0; j<RC; j++)
      if (i==j)
	m[i][j] = 1;
      else
	m[i][j] = 0;
}

template <typename T, unsigned RC>
const Matrix<T, RC, RC> inv (const Matrix<T, RC, RC>& m)
{
  Matrix<T, RC, RC> copy (m);
  
  Matrix<T, RC, RC> inverse;
  identity (inverse);

  GaussJordan (copy, inverse);

  return inverse;
}

template <typename T, unsigned Rows, unsigned Columns>
const Matrix<T, Columns, Rows> transpose (const Matrix<T, Rows, Columns>& m)
{
  Matrix<T, Columns, Rows> result;

  for (unsigned i=0; i<Rows; i++)
    for (unsigned j=0; j<Columns; j++)
      result[j][i] = m[i][j];

  return result;
}

#endif  /* not __Matrix_H defined */

