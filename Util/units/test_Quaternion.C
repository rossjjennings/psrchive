#include "FourSpaceTest.h"
#include "Quaternion.h"

// This must be defined for the FourSpaceTest
float norm (float x)
{
  return x*x;
}

int main () 
{
  unsigned testloops = 10;

  FourSpaceTest <Quaternion<float,Unitary>, 
    Quaternion<double,Unitary>, float > testu;

  try {
    cerr << "Testing Unitary Quaternion operations" << endl;
    testu.runtest (testloops);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }




  FourSpaceTest <Quaternion<float,Hermitian>, 
    Quaternion<double,Hermitian>, float > testh;

  try {
    cerr << "Testing Hermitian Quaternion operations" << endl;
    testh.runtest (testloops);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
