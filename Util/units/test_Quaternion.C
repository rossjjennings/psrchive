#include "FourSpaceTest.h"
#include "Quaternion.h"

// This must be defined for the FourSpaceTest
float norm (float x)
{
  return x*x;
}

int main () 
{
  unsigned loops = RAND_MAX/1000;

  FourSpaceTest <Quaternion<float,Unitary>, 
    Quaternion<double,Unitary>, float > testu;

  try {
    cerr << "Testing " << loops << " Unitary Quaternion variations" << endl;
    testu.runtest (loops);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }




  FourSpaceTest <Quaternion<complex<float>,Hermitian>, 
    Quaternion<complex<double>,Hermitian>, complex<float> > testh;

  try {
    cerr 
      << "Testing " << loops << " Hermitian Biquaternion variations" << endl;
    testh.runtest (loops);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
