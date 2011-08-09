/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/SpinorJones.h"
#include "MEAL/JonesSpinor.h"

#include "random.h"

#include <iostream>
#include <vector>

using namespace std;

void runtest ()
{
  // the random unit vector to which to fit
  Vector<3,double> target;
  random_vector (target, 10.0);

  MEAL::SingularCoherency start;
  start.set_vector (target);

  MEAL::JonesSpinor spinor;
  spinor.set_model (&start);
}

int main (int argc, char** argv) try
{
  for (unsigned i=0; i < 1000; i++)
    runtest ();

  cerr << "\n\nALL TESTS PASSED\n\n" << endl;

  return 0;
 }
catch (Error & error)
{
  cerr << "test_UnitTangent: error" << error << endl;
  return -1;
}
