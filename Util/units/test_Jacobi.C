
#include "MatrixTest.h"
#include "Jacobi.h"

// specialize for complex matrices
template <typename T, unsigned RC>
void test_Jacobi (Matrix<T,RC,RC>& matrix, float tolerance)
{
  unsigned i,j;

  // make the incoming matrix Hermitian
  for (i=0; i<RC; i++) {
    matrix[i][i] = real(matrix[i][i]);
    for (j=i+1; j<RC; j++)
      matrix[i][j] = conj(matrix[j][i]);
  }

  //cerr << "matrix = " << matrix << endl << endl;

  // make a copy
  Matrix<T, RC, RC> temp = matrix;

  Matrix<T, RC, RC>  eigenvectors;
  Vector<double, RC> eigenvalues;

  Jacobi (temp, eigenvectors, eigenvalues);

  //cerr << "vec=" << eigenvectors << endl;
  //cerr << "val=" << eigenvalues << endl;

  //temp = herm(eigenvectors) * matrix * eigenvectors;
  temp = eigenvectors * matrix * herm(eigenvectors);

  //cerr << "temp=" << temp << endl;

  // ensure that the result is diagonal and that the diagonal elements
  // are equal to the eigenvalues

  for (i=0; i<RC; i++)
    for (j=0; j<RC; j++) {

      double should_be = 0;
      if (i == j)
	should_be = eigenvalues[i];

      if (norm(temp[i][j] - should_be) > tolerance) {
	cerr << "test_Jacobi t[" << i << "][" << j << "]=" << temp[i][j]
	     << " != " << should_be << endl;

	cerr << "P = " << eigenvectors << endl;
	cerr << "P^T A P = " << temp << endl;

	throw string ("test_Jacobi error");
      }
    }

}

template<typename T, unsigned dim> 
void runtest (unsigned loops, float tolerance)
{
  cerr << loops << " " << dim << "x" << dim << " Jacobi solutions" << endl;

  for (unsigned iloop=0; iloop<loops; iloop++) {

    Matrix<T,dim,dim> m1;
    random_matrix (m1, 10.0);

    test_Jacobi (m1, tolerance);

  }
}

int main () try {

  unsigned loops = 100000;
  float tolerance = 1e-12;

  cerr << "Testing symmetric: ";
  runtest<double,7> (loops, tolerance);

  cerr << "Testing Hermitian: ";
  runtest<complex<double>,5> (loops, tolerance);

  cerr << "Successful tests" << endl;
  return 0;

}
catch (string& error) {
  cerr << "test_Jacobi error "<< error << endl;
  return -1;
}
