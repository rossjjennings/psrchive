#include "Calibration/BinaryScalar.h"
#include "Calibration/ProjectGradient.h"

void Calibration::BinaryScalar::calculate (double& result,
					   vector<double>* grad)
{
  if (!arg1 || !arg2)
    throw Error (InvalidState, "Calibration::BinaryScalar::calculate",
		 "both arguments are not set");

  if (verbose)
    cerr << "Calibration::BinaryScalar::calculate" << endl;

  vector<double> grad1;
  vector<double> grad2;

  vector<double>* grad1_ptr = 0;
  vector<double>* grad2_ptr = 0;

  if (grad)  {
    grad1_ptr = &grad1;
    grad2_ptr = &grad2;
  }

  double x1 = arg1->evaluate (grad1_ptr);
  double x2 = arg2->evaluate (grad2_ptr);

  result = function( x1, x2 );

  if (verbose)
    cerr << "Calibration::BinaryScalar::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  double dydx1 = partial_arg1 ( x1, x2 );
  double dydx2 = partial_arg2 ( x1, x2 );

  unsigned igrad;
  for (igrad=0; igrad<grad1.size(); igrad++)
    grad1[igrad] *= dydx1;

  for (igrad=0; igrad<grad2.size(); igrad++)
    grad2[igrad] *= dydx2;

  unsigned ngrad = get_nparam();
  grad->resize (ngrad);

  for (igrad=0; igrad<ngrad; igrad++)
    (*grad)[igrad] = 0.0;

  // map the model gradient
  ProjectGradient (arg1, grad1, *(grad));
  ProjectGradient (arg2, grad2, *(grad));

  if (verbose) {
    cerr << "Calibration::BinaryScalar::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}
