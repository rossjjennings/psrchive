#include "MatrixTest.h"
#include "Pauli.h"

// specialize the test_matrix template function defined in MatrixTest.h
void test_matrix (const Jones<float>& j1, const Jones<float>& j2,
		  const Quaternion<float>& q, const complex<float>& c,
		  bool verbose)
{
  if (verbose)
    cerr << "test_matrix Jones/Quaternion Isomorphism" << endl;

  // pull the jones matrices apart into their Hermitian and Unitary components

  complex<float> d1;
  Quaternion<float, Hermitian> hq1;
  Quaternion<float, Unitary> uq1;

  polar (d1, hq1, uq1, j1);

  complex<float> d2;
  Quaternion<float, Hermitian> hq2;
  Quaternion<float, Unitary> uq2;

  polar (d2, hq2, uq2, j2);

  // reconstruct the Jones matrices

  Jones<float> oj1 = d1 * convert(hq1) * convert(uq1);

  Jones<float> oj2 = d2 * convert(hq2) * convert(uq2);

  // test that you get the same matrix back
  double diff = norm(oj1-j1)/norm(oj1);
  if ( diff > 1e-15 ) {
    cerr << "test_matrix polar recomposition=" << oj1 
	 << " != Jones=" << j1 << endl
	 << " by " << diff;
    throw string ("test_matrix polar decomposition error");
  }

  diff = norm(oj2-j2)/norm(oj2);
  if ( diff > 1e-15 ) {
    cerr << "test_matrix polar recomposition=" << oj2 
	 << " != Jones=" << j2 << endl
	 << " by " << diff;
    throw string ("test_matrix polar decomposition error");
  }

  Jones<float> uj1 = convert (uq1);
  Jones<float> uj2 = convert (uq2);

#if 0
  Quaternion<float> pq1 = uq1 * uq2;
  Quaternion<float> pq2 = convert (uj1 * uj2);

  if ( norm(pq2-pq1)/norm(pq2) > 1e-15 ) {
    cerr << "test_matrix pq2=" << pq2 
	 << " != pq1=" << pq1 << endl;
    throw string ("test_matrix unequal Quaternion/Jones products");
  }
#endif

  Jones<float> pj1 = uj2 * uj1;
  Jones<float> pj2 = convert (uq2 * uq1);

  diff = norm(pj2-pj1)/norm(pj2);
  if ( diff > 1e-15 ) {
    cerr << "test_matrix pj2=" << pj2 
	 << " != pj1=" << pj1 << endl
	 << " by " << diff << endl;
    throw string ("test_matrix unequal Jones/Quaternion products");
  }

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
  cerr << "IN j0=" << j0 << endl;

  complex<float> d0;
  polar (d0, hq0, uq0, j0);

  hj0 = convert(hq0);
  uj0 = convert(uq0);

  j0 = d0 * hj0 * uj0;

  cerr << "OUT j0=" << j0 << endl;


  MatrixTest <Jones<float>, Quaternion<float>, complex<float> > test;

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
