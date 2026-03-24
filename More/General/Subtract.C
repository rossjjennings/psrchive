/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Subtract.h"
#include "Pulsar/Profile.h"
#include "ChiSquared.h"

using namespace std;

void Pulsar::Subtract::transform (Profile* profile)
{
  if (!subtract_linear_fit)
  {
    profile->diff( get_operand() );
    return;
  }

  //! Used to compute the residual
  BinaryStatistics::ChiSquared chi;

  vector<double> amps;
  profile->get_amps(amps);
  vector<double> operand_amps;
  get_operand()->get_amps(operand_amps);

  chi.set_outlier_threshold (0.0);
  double chisq = chi.get (amps, operand_amps);

  if (Profile::verbose)
    cerr << "Subtract::transform chi-squared = " << chisq << endl;

  vector<double> residual = chi.get_residual();

  profile->set_amps(residual);
}
