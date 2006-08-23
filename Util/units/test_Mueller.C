#include "MEAL/Polar.h"
#include "MEAL/RandomPolar.h"

#include "Pauli.h"

#include <iostream>

using namespace std;

int main ()
{
  MEAL::RandomPolar random;

  for (unsigned i=0; i < 1000; i++) {

    MEAL::Polar polar;
    random.get (&polar);

    Jones<double> J = polar.evaluate();

    Stokes<double> input;

    random_vector (input, 10.0);

    Stokes<double> output1 = transform (input, J);

    Matrix<4,4,double> M = Mueller (J);

    Stokes<double> output2 = M * input;

    Stokes<double> diff = output2 - output1;

    if (norm(diff)/norm(output1) > 1e-15) {
      cerr << "Error: out1=" << output1 << endl
	   << "    != out2=" << output2 << endl;
      return -1;
    }
  }

  cerr << "Mueller passes test" << endl;

  return 0;
}
