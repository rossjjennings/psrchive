/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ComplexCorrelation.h"
#include "MEAL/Parameters.h"
#include "MEAL/ScalarAtanc.h"

using namespace std;

MEAL::ComplexCorrelation::ComplexCorrelation ()
{
  Parameters* params = new Parameters (this, 2);
  params->set_name (0, "real");
  params->set_name (1, "imag");
}

//! Calculate the Jones matrix and its gradient
void MEAL::ComplexCorrelation::calculate (Result& result,
					vector<Result>* grad)
{
  std::complex<double> z ( get_param(0), get_param(1) );
  double mod_z = abs (z);

  double datancc_z = 0;
  double atanc_z = atanc (mod_z, NULL, &datancc_z);

  result = z * atanc_z;

  if (verbose)
    cerr << "MEAL::ComplexCorrelation::calculate z=" << result << endl;

  if (grad)
  {
    (*grad)[0] = std::complex<double>( atanc_z, 0 )
      + z * datancc_z * z.real();

    (*grad)[1] = std::complex<double>( 0, atanc_z )
      + z * datancc_z * z.imag();
  }
}
