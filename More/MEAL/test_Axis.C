#include "Calibration/Axis.h"
#include "Calibration/Polynomial.h"

int main (int argc, char** argv)
{ try {

  Calibration::Model::verbose = true;

  cerr << "Instantiating Calibration::Polynomial" << endl;
  Calibration::Polynomial poly (2);
  poly.set_param (1, 1.0);

  cerr << "Instantiating Calibration::Axis<double>" << endl;
  Calibration::Axis<double> axis;

  cerr << "Connecting Calibration::Polynomial::set_abscissa"
    " to Calibration::Axis<double>" << endl;

  poly.set_argument (0, &axis);

  double test_value = 3.4;

  cerr << "Instantiating Calibration::Axis<double>::Value" << endl;

  Calibration::Argument::Value* abscissa = axis.new_Value (test_value);

  if (poly.get_abscissa() != 0.0) {
    cerr << "Error Polynomial::get_abscissa=" << poly.get_abscissa() << " != 0"
	 << endl;
    return -1;
  }

  cerr << "Calling Calibration::Axis<double>::apply" << endl;
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

  cerr << "Calibration::Feed constructor passes test" << endl;
  return 0;
}
