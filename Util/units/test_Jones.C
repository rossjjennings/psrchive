#include "FourSpaceTest.h"
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
  unsigned testloops = 10;

  FourSpaceTest <Jones<float>, Jones<double>, complex<float> > test;

  try {
    cerr << "Testing Jones matrix operations" << endl;
    test.runtest (testloops);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
