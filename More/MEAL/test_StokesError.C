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

  Stokes<double> output = test.get_variance ();

  if (input != output) {
    cerr << "FAIL! identity\n"
      "input = " << input << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple identity test passed" << endl;

  MEAL::Rotation rotation;
  rotation.set_param (0, M_PI/4);

  test.set_transformation (rotation.evaluate());
  output = test.get_variance ();

  Stokes<double> expect (1,2,.6,.5);

  if (expect != output) {
    cerr << "FAIL! simple rotation\n"
      "expect = " << expect << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple rotation test passed" << endl;

  J = 0;
  test.set_transformation_gradient (J);
  output = test.get_variance_gradient ();

  expect = 0;
  if (expect != output) {
    cerr << "FAIL! null gradient\n"
      "expect = " << expect << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple null gradient test passed" << endl;

  vector< Jones<double> > grad;

  Stokes<double> input2 (3.63323e-08,3.41734e-08,3.57376e-08,3.55304e-08);

  test.set_variance (input2);
  test.set_transformation( rotation.evaluate(&grad) );

  test.set_transformation_gradient (grad[0]);
  output = test.get_variance_gradient ();

  if (output[2] == 0 || output[3] == 0) {
    cerr << "FAIL! non-zero gradient\n"
      "gradient = " << grad[0] << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple non-zero gradient test passed" << endl;

  double delta_phi = 1e-3;

  test.set_transformation( rotation.evaluate() );
  output = test.get_variance ();

  rotation.set_param(0, rotation.get_param(0) + delta_phi);
  test.set_transformation( rotation.evaluate() );

  Stokes<double> output2 = test.get_variance ();
  Stokes<double> diff = output2 - output;

  rotation.set_param(0, rotation.get_param(0) - 0.5*delta_phi);
  test.set_transformation( rotation.evaluate(&grad) );
  test.set_transformation_gradient (grad[0]);

  expect = delta_phi * test.get_variance_gradient ();

  double ndiff = norm(diff - expect) / norm(diff);

  if (ndiff > 1e-6) {
    cerr << "FAIL! differential gradient test\n"
      "expect = " << expect << "\n"
      "diff = " << diff << endl;
    return -1;
  }

  cerr << "differential gradient test passed" << endl;

  cerr << "all tests passed" << endl;
  return 0;
}
