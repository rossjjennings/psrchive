#include "MatrixTest.h"
#include "Pauli.h"

template <typename T> T sqr (T x) { return x*x; }

template <typename T>
void test_polar (const Jones<T>& j, float tolerance)
{
  // pull the jones matrix apart into its Hermitian and Unitary components
  complex<T> d;
  Quaternion<T, Hermitian> hq;
  Quaternion<T, Unitary> uq;

  polar (d, hq, uq, j);

  double determinant = 0;

  determinant = det(hq);
  if ( sqr(determinant - 1.0) > tolerance ) {
    cerr << "test_matrix polar det(hermitian)=" << determinant <<" != 1"<<endl;
    throw string ("test_matrix polar decomposition error");
  }

  determinant = det(uq);
  if ( sqr(determinant - 1.0) > tolerance ) {
    cerr << "test_matrix polar det(unitary)=" << determinant <<" != 1"<<endl;
    throw string ("test_matrix polar decomposition error");
  }


  // reconstruct the Jones matrix
  Jones<T> oj = d * (hq * uq);

  // test that you get the same matrix back
  double diff = norm(oj-j)/norm(oj);
  if ( diff > tolerance ) {
    cerr << "test_matrix polar\n"
      " out=" << oj << " !=\n"
      "  in=" << j << endl << " diff=" << diff << endl;
    throw string ("test_matrix polar decomposition error");
  }
}

// specialize the test_matrix template function defined in MatrixTest.h
template <typename T>
void test_matrix (const Jones<T>& j1, const Jones<T>& j2,
		  const Quaternion<T>& q, const complex<T>& c,
		  float scale, float tolerance, bool verbose)
{
  if (verbose)
    cerr << "test_matrix Jones/Quaternion Isomorphism" << endl;

  test_polar (j1, tolerance);
  test_polar (j2, tolerance);

}


int main () 
{
#if (RAND_MAX > 9000000)
  unsigned loops = RAND_MAX/1000;
#else
  unsigned loops = RAND_MAX*10;
#endif

  MatrixTest <Jones<double>, Quaternion<double>, complex<double> > test;

  try {
    cerr << "Testing " << loops << " Pauli conversion variations" << endl;
    test.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
