#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

void Pulsar::GaussianBaseline::get_bounds (PhaseWeight& weight, 
					   float& lower, float& upper)
{
  double mean, var, var_mean;

  weight.stats (profile, &mean, &var, &var_mean);
  double cutoff = threshold * sqrt (var + var_mean);

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::get_bounds "
	 << " baseline mean=" << mean << " cutoff=" << cutoff
	 << " = " << threshold << " sigma" << endl;

  lower = mean - cutoff;
  upper = mean + cutoff;
}
