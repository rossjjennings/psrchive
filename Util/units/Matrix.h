//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Matrix.h,v $
   $Revision: 1.1 $
   $Date: 2003/03/03 09:56:43 $
   $Author: straten $ */

#ifndef __Matrix_H
#define __Matrix_H

#include <iostream>

#include "Vector.h"
#include "psr_cpp.h"

//! Matrix is a column Column-vector of row Row-vectors
template <typename T, unsigned Column, unsigned Row> 
class Matrix : public Vector< Vector<T, Row>, Column > {
  
public:

  //! Null constructor
  Matrix () { for (unsigned i=0; i<Column; i++) x[i] = 0; }

  //! Construct from another Matrix<U> instance
  template<typename U> Matrix (const Matrix<U, Column, Row>& s)
  { operator=(s); }

  //! Set this instance equal to another Matrix<U> instance
  template<typename U> Matrix& operator = (const Matrix<U, Column, Row>& s)
  { for (unsigned i=0; i<Column; i++) x[i] = s.x[i]; return *this; }
  
  //! Vector multiplication
  const Vector<T, Column> operator * (const Vector<T, Row>& b)
  { 
    Vector<T, Column> r; 
    for (unsigned i=0; i<Column; i++)
      r[i] = x[i] * b;
    return r;
  }

};

//! Matrix multiplication
template<typename T, unsigned C1, unsigned R1C2, unsigned R2>
const Matrix<T, C1, R2>
operator * (const Matrix<T, C1, R1C2>& a, const Matrix<T, R1C2, R2>& b)
{ 
  Matrix<T, C1, R2> r; 
  for (unsigned i=0; i<C1; i++)
    for (unsigned j=0; j<R2; j++)
      for (unsigned k=0; k<R1C2; k++)
	r[i][j] += a[i][k]*b[k][j];
  return r;
}

#endif  /* not __Matrix_H defined */

