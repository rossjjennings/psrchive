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




  FourSpaceTest <Quaternion<float,Hermitian>, 
    Quaternion<double,Hermitian>, float > testh;

  try {
    cerr << "Testing " << loops << " Hermitian Quaternion variations" << endl;
    testh.runtest (loops);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
