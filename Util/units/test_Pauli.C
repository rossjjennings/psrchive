#include "MatrixTest.h"
#include "Pauli.h"

template <typename T>
void test_polar (const Jones<T>& j)
{
  // pull the jones matrix apart into its Hermitian and Unitary components
  complex<T> d;
  Quaternion<T, Hermitian> hq;
  Quaternion<T, Unitary> uq;

  polar (d, hq, uq, j);

  double determinant = 0;

  determinant = det(hq);
  if ( fabs(determinant - 1.0) > 1e-8 ) {
    cerr << "test_matrix polar det(hermitian)=" << determinant <<" != 1"<<endl;
    throw string ("test_matrix polar decomposition error");
  }

  determinant = det(uq);
  if ( fabs(determinant - 1.0) > 1e-8 ) {
    cerr << "test_matrix polar det(unitary)=" << determinant <<" != 1"<<endl;
    throw string ("test_matrix polar decomposition error");
  }


  // reconstruct the Jones matrix
  Jones<T> oj = d * convert(hq) * convert(uq);

  // test that you get the same matrix back
  double diff = norm(oj-j)/norm(oj);
  if ( diff > 1e-10 ) {
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
		  bool verbose)
{
  if (verbose)
    cerr << "test_matrix Jones/Quaternion Isomorphism" << endl;

  test_polar (j1);
  test_polar (j2);

}


int main () 
{
#if (RAND_MAX > 9000000)
  unsigned loops = RAND_MAX/1000;
#else
  unsigned loops = RAND_MAX*10;
#endif



  Quaternion<float, Hermitian> hq0 (1,2,3,4);
  Quaternion<float, Unitary> uq0 (5,6,7,8);

  cerr << "hq=" << hq0 << " uq=" << uq0 << endl;

  Jones<float> hj0 = convert(hq0);
  Jones<float> uj0 = convert(uq0);

  cerr << "hj=" << hj0 << " uj=" << uj0 << endl;

  Jones<float> j0 = hj0 * uj0;
  cerr << " IN j0=" << j0 << endl;

  complex<float> d0;
  polar (d0, hq0, uq0, j0);

  hj0 = convert(hq0);
  uj0 = convert(uq0);

  j0 = d0 * hj0 * uj0;

  cerr << "OUT j0=" << j0 << endl;


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
