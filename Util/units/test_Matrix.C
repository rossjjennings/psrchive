#include "Matrix.h"
#include "random.h"

int main ()
{
  Matrix<float, 3, 3> m1;
  Matrix<float, 3, 3> m2;
  Vector<float, 3> v;

  random_matrix (m1, 10.0);
  random_matrix (m2, 10.0);
  random_vector (v, 10.0);

  cerr << "m1=" << m1 << " m2=" << m2 << " m1+m2=" << m1+m2 
       << " m1*m2=" << m1*m2 << endl;

  cerr << "v=" << v << " m1*v=" << m1*v << endl;

  return 0;
}

