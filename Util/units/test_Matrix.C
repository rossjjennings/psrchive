#include "Matrix.h"
#include "random.h"

using namespace std;

int main ()
{
  Matrix<float, 3, 4> m1;
  Matrix<float, 4, 5> m2;
  Vector<float, 4> v;

  random_matrix (m1, 10.0);
  random_matrix (m2, 10.0);
  random_vector (v, 10.0);

  cerr << "m1=" << m1 << "\nm2=" << m2 << "\nm1*m2=" << m1*m2 << endl;

  cerr << "v=" << v << " m1*v=" << m1*v << endl;

  Matrix<double, 3, 3> test;
  random_matrix (test, 10.0);

  Matrix<double, 3, 3> inverse = inv (test);

  cerr << "M*inv(M)=" << test * inverse << endl;

  return 0;
}

