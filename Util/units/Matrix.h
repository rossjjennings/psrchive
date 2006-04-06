//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Matrix.h,v $
   $Revision: 1.15 $
   $Date: 2006/04/06 19:13:41 $
   $Author: straten $ */

#ifndef __Matrix_H
#define __Matrix_H

#include <iostream>

#include "Vector.h"
#include "Error.h"

//! Matrix is a column vector of row vectors
template <unsigned Rows, unsigned Columns, typename T> 
class Matrix : public Vector< Rows, Vector<Columns,T> > {
  
public:

  //! Null constructor
  Matrix () { for (unsigned i=0; i<Rows; i++) this->x[i] = 0; }

  //! Construct from another Matrix<U> instance
  template<typename U> Matrix (const Matrix<Rows, Columns, U>& s)
  { operator=(s); }

  //! Set this instance equal to another Matrix<U> instance
  template<typename U> Matrix& operator = (const Matrix<Rows,Columns,U>& s)
  { for (unsigned i=0; i<Rows; i++) this->x[i] = s.x[i]; return *this; }
  
  //! Negation
  const friend Matrix operator - (Matrix s)
  { for (unsigned i=0; i<Rows; i++) for (unsigned j=0; j<Columns; j++)
    s.x[i][j] = -s.x[i][j]; return s; }
  
};

//! Vector multiplication
template<unsigned Rows, unsigned Columns, typename T, typename U>
const Vector<Rows,U> operator * (const Matrix<Rows,Columns,T>& m,
                                 const Vector<Columns,U>& b)
{
  Vector<Rows,U> r;
  for (unsigned i=0; i<Rows; i++)
    r[i] = Vector<Columns,U>(m[i]) * b;
  return r;
}

//! Vector transpose multiplication
template<unsigned Rows, unsigned Columns, typename T, typename U>
const Vector<Columns,U> operator * (const Vector<Rows,U>& b,
				    const Matrix<Rows,Columns,T>& m)
{
  Vector<Columns,U> r;
  for (unsigned j=0; j<Columns; j++)
    for (unsigned i=0; i<Rows; i++)
      r[j] += m[i][j] * b[i];
  return r;
}

//! Matrix multiplication
template<unsigned R1, unsigned C1R2, unsigned C2, typename T>
const Matrix<R1, C2, T>
operator * (const Matrix<R1, C1R2, T>& a, const Matrix<C1R2, C2, T>& b)
{ 
  Matrix<R1, C2, T> r; 
  for (unsigned i=0; i<R1; i++)
    for (unsigned j=0; j<C2; j++)
      for (unsigned k=0; k<C1R2; k++)
	r[i][j] += a[i][k]*b[k][j];
  return r;
}

template <unsigned Rows, unsigned C1, typename T, unsigned C2, typename U>
void GaussJordan (Matrix<Rows,C1,T>& a, Matrix<Rows,C2,U>& b)
{
  Vector<Rows, unsigned> indxc;
  Vector<Rows, unsigned> indxr;
  Vector<Rows, unsigned> ipiv;

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
	    throw Error (InvalidParam, "GaussJordan (Matrix)",
			 "Singular Matrix-1");
	}
    }

    //cerr << "1" << endl;

    ipiv[icol]++;
    
    if (irow != icol) {
      for (j=0; j<Rows; j++)
	std::swap (a[irow][j], a[icol][j]);
      for (j=0; j<C2; j++)
	std::swap (b[irow][j], b[icol][j]);
    }

    //cerr << "2" << endl;

    indxr[i]=irow;
    indxc[i]=icol;

    if (a[icol][icol] == 0.0)
      throw Error (InvalidParam, "GaussJordan (Matrix)", "Singular Matrix-2");

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
	std::swap(a[j][indxr[i-1]],a[j][indxc[i-1]]);
  }
  
   //cerr << "7" << endl;
 
}

template <unsigned RC, typename T>
void matrix_identity (Matrix<RC, RC, T>& m)
{
  for (unsigned i=0; i<RC; i++)
    for (unsigned j=0; j<RC; j++)
      if (i==j)
	m[i][j] = 1;
      else
	m[i][j] = 0;
}

template <unsigned RC, typename T>
const Matrix<RC, RC, T> inv (const Matrix<RC, RC, T>& m)
{
  Matrix<RC, RC, T> copy (m);
  
  Matrix<RC, RC, T> inverse;
  matrix_identity (inverse);

  GaussJordan (copy, inverse);

  return inverse;
}

template <unsigned Rows, unsigned Columns, typename T>
const Matrix<Columns, Rows, T> transpose (const Matrix<Rows, Columns,T>& m)
{
  Matrix< Columns, Rows,T> result;

  for (unsigned i=0; i<Rows; i++)
    for (unsigned j=0; j<Columns; j++)
      result[j][i] = m[i][j];

  return result;
}

template <unsigned Rows, unsigned Columns, typename T>
const Matrix< Columns, Rows,T> herm (const Matrix<Rows, Columns,T>& m)
{
  Matrix< Columns, Rows,T> result;

  for (unsigned i=0; i<Rows; i++)
    for (unsigned j=0; j<Columns; j++)
      result[j][i] = conj( m[i][j] );

  return result;
}

//! Vector direct (outer) product 
template<unsigned Rows, unsigned Columns, typename T, typename U>
const Matrix<Rows,Columns,T> outer (const Vector<Rows,T>& a,
				    const Vector<Columns,U>& b)
{
  Matrix<Rows, Columns, T> result;

  for (unsigned i=0; i<Rows; i++)
    for (unsigned j=0; j<Columns; j++)
      result[i][j] = a[i] * b[j];

  return result;
}

//! Useful for printing the components
template<unsigned R, unsigned C, typename T>
std::ostream& operator<< (std::ostream& ostr, const Matrix<R,C,T>& m)
{
  ostr << "[" << m[0];
  for (unsigned i=1; i<R; i++)
    ostr << ",\n " << m[i];
  return ostr << "]";
}

#endif  /* not __Matrix_H defined */

