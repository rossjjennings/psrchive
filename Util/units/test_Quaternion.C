#include "MatrixTest.h"
#include "Quaternion.h"

// This must be defined for the MatrixTest
float norm (float x)
{
  return x*x;
}

int main () 
{
#if (RAND_MAX > 9000000)
  unsigned loops = RAND_MAX/1000;
#else
  unsigned loops = RAND_MAX*1000;
#endif

  MatrixTest <Quaternion<float,Unitary>,
    Quaternion<double,Unitary>, float> testu;

  try {
    cerr << "Testing " << loops << " Unitary Quaternion variations" << endl;
    testu.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }




  MatrixTest <Quaternion<complex<float>,Hermitian>, 
    Quaternion<complex<double>,Hermitian>, complex<float> > testh;

  try {
    cerr 
      << "Testing " << loops << " Hermitian Biquaternion variations" << endl;
    testh.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
