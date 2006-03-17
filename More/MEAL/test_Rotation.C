/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Rotation.h"
#include "Pauli.h"

#include <iostream>
using namespace std;

static char* name = "QUV";

void test_Rotation (double theta, double phi, unsigned axis, unsigned perm=0)
{
  MEAL::Rotation rotation;

  rotation.set_axis (Vector<3,float>::basis(axis));
  rotation.set_param (0, phi);

  Jones<double> xform = rotation.evaluate();

  Stokes<double> stokes (1.5);
  unsigned i = (axis+perm+1)%3;
  unsigned j = (axis+perm+2)%3;
  unsigned k = (axis+perm)%3;

  stokes[i+1] = cos(theta);
  stokes[j+1] = sin(theta);
  stokes[k+1] = 0.2;

  stokes = transform (stokes, xform);

  double result = theta - 2.0*phi;

  if (stokes[0] != 1.5)
    cerr << "I=" << stokes[0] << " != " << 1.5 << endl;

  if (fabs (stokes[i+1] - cos(result)) > 1e-12) {
    cerr << name[i] << "=" << stokes[i+1] << " != " << cos(result) << endl;
    exit(-1);
  }

  if (fabs (stokes[j+1] - sin(result)) > 1e-12) {
    cerr << name[j] << "=" << stokes[j+1] << " != " << sin(result) << endl;
      exit(-1);
  }

  if (fabs (stokes[k+1] - 0.2) > 1e-12) {
    cerr << name[k] << "=" << stokes[k+1] << " != " << 0.2 << endl;
    exit(-1);
  }

}

int main ()
{
  cerr << "Testing in linear basis" << endl;
  for (unsigned i=0; i < 3; i++) {
    cerr << "Testing rotations about " << name[i] << " axis" << endl;
    test_Rotation (1.43/2, -2.15/2, i);
  }

  cerr << "Testing in circular basis" << endl;
  Pauli::basis.set_basis (Signal::Circular);

  for (unsigned i=0; i < 3; i++) {
    cerr << "Testing rotations about " << name[(i+2)%3] << " axis" << endl;
    test_Rotation (-.73/2, 3.1/2, i, 2);
  }

  cerr << "All tests passed" << endl;
  return 0;
}

