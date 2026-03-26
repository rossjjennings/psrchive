/***************************************************************************
 *
 *   Copyright (C) 2007 - 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Subtract.h"
#include "Pulsar/Profile.h"
#include "ChiSquared.h"

using namespace std;

void Pulsar::Subtract::transform (Profile* profile)
{
  if (fit == None)
  {
    profile->diff( get_operand() );
    return;
  }

  //! Used to compute the residual
  BinaryStatistics::ChiSquared chi;

  vector<double> left_amps;
  vector<double> right_amps;

  profile->get_amps(left_amps);
  get_operand()->get_amps(right_amps);

  chi.set_outlier_threshold (0.0);
  double chisq = 0.0;

  if (fit == LeftToRight)
  {
    chisq = chi.get (right_amps, left_amps);
  }
  else if (fit == RightToLeft)
  {
    chisq = chi.get (left_amps, right_amps);
  }

  if (Profile::verbose)
    cerr << "Subtract::transform chi-squared = " << chisq << endl;

  vector<double> residual = chi.get_residual();

  profile->set_amps(residual);

  if (fit == LeftToRight)
    profile->scale(-1.0);
}
