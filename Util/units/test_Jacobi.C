
#include "MatrixTest.h"
#include "Jacobi.h"

template <typename T, unsigned RC>
void test_Jacobi (Matrix<T,RC,RC>& matrix, float tolerance)
{
  unsigned i,j;

  // make the incoming matrix symmetric
  for (i=0; i<RC; i++)
    for (j=i+1; j<RC; j++)
      matrix[i][j] = matrix[j][i];

  //cerr << "matrix = " << matrix << endl << endl;

  // make a copy
  Matrix<T, RC, RC> temp = matrix;

  Matrix<T, RC, RC> eigenvectors;
  Vector<T, RC>     eigenvalues;
  Jacobi (temp, eigenvectors, eigenvalues);

  //cerr << "vec=" << eigenvectors << endl;
  //cerr << "val=" << eigenvalues << endl;

  temp = transpose(eigenvectors) * matrix * eigenvectors;

  //cerr << "temp=" << temp << endl;

  // ensure that the result is diagonal and that the diagonal elements
  // are equal to the eigenvalues

  for (i=0; i<RC; i++)
    for (j=0; j<RC; j++) {

      T should_be = 0;
      if (i == j)
	should_be = eigenvalues[i];

      if (fabs(temp[i][j] - should_be) > tolerance) {
	cerr << "test_Jacobi t[" << i << "][" << j << "]=" << temp[i][j]
	     << " != " << should_be << endl;
	throw string ("test_Jacobi error");
      }
    }

}


int main () 
{
  unsigned loops = 100000;

  float tolerance = 1e-12;
  cerr << "Testing " << loops << " symmetric 7x7 Jacobi solutions" << endl;

  for (unsigned iloop=0; iloop<loops; iloop++) {

    Matrix<double,7,7> m1;
    random_matrix (m1, 10.0);

    try {
      test_Jacobi (m1, tolerance);
    }
    catch (string& error) {
      cerr << "test_Jacobi error on loop "<< iloop <<"/"<< loops 
	   << endl;
      return -1;
    }

  }



  return 0;
}
