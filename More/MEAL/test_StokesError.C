#include "MEAL/StokesError.h"

#include "MEAL/Rotation.h"
#include "MEAL/Polar.h"
#include "MEAL/RandomPolar.h"

#include "Pauli.h"

#include <iostream>

using namespace std;

int main ()
{
  MEAL::StokesError test;

  // test the identity
  Jones<double> J = 1;
  test.set_transformation (J);

  Stokes<double> input (1,2,.5,.6);
  test.set_variance (input);

  Stokes<double> output;
  test.get_variance (output);

  if (input != output) {
    cerr << "FAIL!\n"
      "input = " << input << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple identity test passed" << endl;

  MEAL::Rotation rotation;
  rotation.set_param (0, M_PI/4);

  test.set_transformation (rotation.evaluate());
  test.get_variance (output);

  Stokes<double> expect (1,2,.6,.5);

  if (expect != output) {
    cerr << "FAIL!\n"
      "expect = " << expect << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple rotation test passed" << endl;

  cerr << "testing that this was not a huge waste of time" << endl;

  MEAL::RandomPolar random;

  for (unsigned i=0; i < 1000; i++) {

    MEAL::Polar polar;
    random.get (&polar);

    J = polar.evaluate();
    test.set_transformation (J);

    test.get_variance (output);

    // ... is it a simple matter of propagating the error, sigma ??

    Stokes<double> temp;
    for (unsigned i=0; i<4; i++)
      temp[i] = sqrt(input[i]);

    Stokes<double> expect = transform (input, J);

    for (unsigned i=0; i<4; i++)
      expect[i] *= expect[i];

    if (expect == output) {
      cerr << "INTERESTING!\n"
	"expect = " << expect << "\n"
	"output= " << output << endl;
      return -1;
    }

  }

  cerr << "time apparently not wasted" << endl;

  return 0;
}
