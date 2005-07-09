#include "MEAL/Rotation.h"
#include "Pauli.h"

#include <iostream>
using namespace std;

static char* name = "QUV";

void test_Rotation (double theta, double phi, unsigned axis)
{
  MEAL::Rotation rotation;

  rotation.set_axis (Vector<float,3>::basis(axis));
  rotation.set_param (0, phi);

  Jones<double> xform = rotation.evaluate();

  Stokes<double> stokes (1.5);
  unsigned i = (axis+1)%3;
  unsigned j = (axis+2)%3;
  unsigned k = axis;

  stokes[i+1] = cos(theta);
  stokes[j+1] = sin(theta);
  stokes[k+1] = 0.2;

  stokes = transform (stokes, xform);

  double result = theta - 2.0*phi;

  if (stokes[0] != 1.5)
    cerr << "I=" << stokes[0] << " != " << 1.5 << endl;

  if (stokes[i+1] != cos(result))
    cerr << name[i] << "=" << stokes[i+1] << " != " << cos(result) << endl;

  if (stokes[j+1] != sin(result))
    cerr << name[j] << "=" << stokes[j+1] << " != " << sin(result) << endl;

  if (stokes[k+1] != 0.2)
    cerr << name[k] << "=" << stokes[k+1] << " != " << 0.2 << endl;

}

int main ()
{
  for (unsigned i=0; i < 3; i++) {
    cerr << "Testing rotations about " << name[i] << " axis" << endl;
    test_Rotation (1.43/2, -2.15/2, i);
  }
  return 0;
}

