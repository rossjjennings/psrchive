//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Cloude.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

#ifndef __Cloude_H
#define __Cloude_H

#include "Pauli.h"
#include "Matrix.h"

//! Return the Hermitian target coherency matrix
/* This method computes the hermitian target coherency matrix, derived from
   the input Jones matrix as described by equations 4.6 and 4.9 of

   Cloude, S.R., 1986, "Group theory and polarization algebra",
   Optik, 75(1), 26-36
*/
template<typename T>
Matrix<std::complex<T>,4,4> coherence (const Jones<T>& jones)
{
  // form the Hermitian biquaternion (Eq. 4.6)
  Quaternion<std::complex<T>,Hermitian> q = convert(jones);

  // convert to the equivalent complex vector and its Hermitian transpose
  Vector<std::complex<T>,4> vect;
  Vector<std::complex<T>,4> herm;

  for (unsigned i=0; i<4; i++) {
    vect[i] = q[i];
    herm[i] = conj(q[i]);
  }

  // Eq. 4.9
  return outer(vect,herm);
}

//! Return the Jones matrix of the target coherency matrix left eigenvector
/* The left eigenvector, as returned by Jacobi, is the row vector
   given by the hermitian transpose of the right (column)
   eigenvector. */
template<typename T>
Jones<T> system (const Vector<std::complex<T>,4>& left_eigen)
{
  Quaternion<std::complex<T>,Hermitian> q;
  
  for (unsigned idim=0; idim<4; idim++)
    q[idim] = conj( left_eigen[idim] );

  return convert (q);
}

//! Return the target entropy, given the target coherency matrix eigenvalues
/*! The eigenvalues are returned by Jacobi as a vector.  The target
  entropy is defined at the end of Section 4 of Cloude (1986). */
template<typename T, unsigned N>
  double entropy (const Vector<T,N>& eigenvalues)
{
  unsigned i;
  double total = 0;

  for (i=0; i<N; i++)
    total += eigenvalues[i];

  double entropy = 0;
  for (i=0; i<N; i++) {
    double Pi = eigenvalues[i]/total;
    entropy -= Pi * log(Pi)/log(double(N));
  }

  return entropy;
}

#endif
