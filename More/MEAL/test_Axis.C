#include "MEAL/Axis.h"
#include "MEAL/Polynomial.h"

using namespace std;

int main (int argc, char** argv)
{ try {

  MEAL::Function::verbose = true;

  cerr << "Instantiating MEAL::Polynomial" << endl;
  MEAL::Polynomial poly (2);
  poly.set_param (1, 1.0);

  cerr << "Instantiating MEAL::Axis<double>" << endl;
  MEAL::Axis<double> axis;

  cerr << "Connecting MEAL::Polynomial::set_abscissa"
    " to MEAL::Axis<double>" << endl;

  poly.set_argument (0, &axis);

  double test_value = 3.4;

  cerr << "Instantiating MEAL::Axis<double>::Value" << endl;

  MEAL::Argument::Value* abscissa = axis.new_Value (test_value);

  if (poly.get_abscissa() != 0.0) {
    cerr << "Error Polynomial::get_abscissa=" << poly.get_abscissa() << " != 0"
	 << endl;
    return -1;
  }

  cerr << "Calling MEAL::Axis<double>::apply" << endl;
  abscissa->apply();

  if (poly.get_abscissa() != test_value) {
    cerr << "Error Polynomial::get_abscissa=" << poly.get_abscissa() << " != "
	 << test_value << endl;
    return -1;
  }

  delete abscissa;

} catch (Error& error) {
  cerr << error << endl;
  return -1;
}

  cerr << "MEAL::Feed constructor passes test" << endl;
  return 0;
}
