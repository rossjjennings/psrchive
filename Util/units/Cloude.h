#include "Pauli.h"
#include "Matrix.h"

//! Return the Hermitian target coherency matrix
/* This method computes the hermitian target coherency matrix, derived from
   the input Jones matrix as described by equations 4.6 and 4.9 of

   Cloude, S.R., 1986, "Group theory and polarization algebra",
   Optik, 75(1), 26-36
*/
template<typename T>
Matrix<complex<T>,4,4> coherence (const Jones<T>& jones)
{
  // form the Hermitian biquaternion (Eq. 4.6)
  Quaternion<complex<T>,Hermitian> q = convert(jones);

  // convert to the equivalent complex vector and its Hermitian transpose
  Vector<complex<T>,4> vect;
  Vector<complex<T>,4> herm;

  for (unsigned i=0; i<4; i++) {
    vect[i] = q[i];
    herm[i] = conj(q[i]);
  }

  // Eq. 4.9
  return outer(vect,herm);
}

//! Return the Jones matrix of the target coherency matrix left eigenvector
/* The left eigenvector is a row vector given by the hermitian transpose of
   the right (column) eigenvector */
template<typename T>
Jones<T> system (const Vector<complex<T>,4>& left_eigen)
{
  Quaternion<complex<T>,Hermitian> q;
  
  for (unsigned idim=0; idim<4; idim++)
    q[idim] = conj( left_eigen[idim] );

  return convert (q);
}
