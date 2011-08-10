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

void compare (const Jones<double>& A, const Jones<double>& B)
{
  if ( norm(A-B)/norm(A+B) > 1e-12 )
  {
    cerr << "A=\n" << A << endl;
    cerr << "B=\n" << B << endl;
    throw Error (InvalidState, "test_Spinor",
		 "matrices are not equal");
  }
}

void runtest ()
{
  // the random unit vector to which to fit
  Vector<3,double> target;
  random_vector (target, 10.0);

  MEAL::SingularCoherency start;
  start.set_vector (target);

  MEAL::JonesSpinor spinor;
  spinor.set_model (&start);

  MEAL::SpinorJones end;
  end.set_spinor (&spinor);

  vector< Jones<double> > grad_start;
  Jones<double> J_start = start.evaluate(&grad_start);

  vector< Jones<double> > grad_end;
  Jones<double> J_end = end.evaluate(&grad_end);

  if (grad_end.size() != grad_start.size())
    throw Error (InvalidState, "test_Spinor",
		 "SingularCoherency grad size=%u != SpinorJones grad size=%u",
		 grad_start.size(), grad_end.size());

  compare (J_start, J_end);

  for (unsigned i=0; i < grad_end.size(); i++)
    compare (grad_start[i], grad_end[i]);
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
