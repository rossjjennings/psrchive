#include "MatrixTest.h"
#include "Jones.h"

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
