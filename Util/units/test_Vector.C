#include "Vector.h"
#include "random.h"

using namespace std;

int main ()
{
  Vector<float, 3> v1;
  Vector<float, 3> v2;

  random_vector (v1, 10.0);
  random_vector (v2, 10.0);

  cerr << "v1=" << v1 << " v2=" << v2 << " v1+v2=" << v1+v2 
       << " v1*v2=" << v1*v2 << endl;

  return 0;
}

