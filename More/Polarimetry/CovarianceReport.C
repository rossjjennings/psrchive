/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CovarianceReport.h"
#include "Pulsar/ReceptionModelSolver.h"

using namespace std;

void Calibration::CovarianceReport::report (ostream& outfile)
{
  auto solver = model->get_solver();
 
  vector< vector<double> > covar;
  solver->get_covariance (covar);

  const unsigned nparam = model->get_nparam();

  if (nparam != covar.size())
    throw Error (InvalidState, "CovarianceReport::report",
                "nparam=%u != covar.size()=%zu", nparam, covar.size());

  for (unsigned i=0; i<nparam; i++)
  {
    double ivar = covar[i][i];
    if (ivar == 0.0)
      continue;

    for (unsigned j=i+1; j<nparam; j++)
    {
      double jvar = covar[j][j];
      if (jvar == 0.0)
        continue;

      outfile << model->get_param_name(i) << " " 
              << model->get_param_name(j) << " "
              << covar[i][j]/sqrt(ivar * jvar) << " " << endl;
    }
  }
} 
