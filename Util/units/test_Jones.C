#include "MatrixTest.h"
#include "Jones.h"

template <class T>
void random (complex<T>& val)
{
  T re, im;

  random(re);
  random(im);

  val = complex<T> (re, im);
}

int main () 
{
  unsigned loops = RAND_MAX/1000;

  MatrixTest <Jones<float>, Jones<double>, complex<float> > test;

  try {
    cerr << "Testing " << loops << " Jones matrix variations" << endl;
    test.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
