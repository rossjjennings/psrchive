#include "Calibration/UnaryScalar.h"

void Calibration::UnaryScalar::calculate (double& result,
					  vector<double>* grad)
{
  if (!model)
    throw Error (InvalidState, "Calibration::UnaryScalar::calculate",
		 "no model to evaluate");

  if (verbose)
    cerr << "Calibration::UnaryScalar::calculate" << endl;

  double x = model->evaluate (grad);

  result = function( x );

  if (verbose)
    cerr << "Calibration::UnaryScalar::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  double dydx = dfdx ( x );

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] *= dydx;

  if (verbose) {
    cerr << "Calibration::UnaryScalar::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}
