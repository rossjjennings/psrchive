/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"
#include "Pulsar/SmoothMean.h"
#include "normal.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Pulsar::GaussianBaseline::GaussianBaseline ()
{
  // The threshold attribute is already set by the IterativeBaseline
  // constructor.  This call sets the moment_correction attribute
  // accordingly.

  set_threshold (1.0);
}

//! Set the threshold below which samples are included in the baseline
void Pulsar::GaussianBaseline::set_threshold (float sigma)
{
  IterativeBaseline::set_threshold (sigma);

  moment_correction = 1.0 / normal_moment2 (threshold);
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::set_threshold sigma=" << sigma
	 << " correction=" << moment_correction << endl;
}

void Pulsar::GaussianBaseline::get_bounds (PhaseWeight& weight, 
					   float& lower, float& upper)
{
  double mean, var, var_mean;

  weight.stats (profile, &mean, &var, &var_mean);

  double effective_variance = var + var_mean;

  if (!get_initial_bounds())
    effective_variance *= moment_correction;

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::get_bounds"
	 << " baseline var=" << var << " var_mean=" << var_mean
	 << " effective=" << effective_variance << endl;

  double cutoff = threshold * sqrt (effective_variance);

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::get_bounds"
	 << " baseline mean=" << mean << " cutoff=" << cutoff
	 << " = " << threshold << " sigma" << endl;

  last_lower = lower = mean - cutoff;
  last_upper = upper = mean + cutoff;
}

void Pulsar::GaussianBaseline::postprocess (PhaseWeight& weight, 
					    const Profile& profile)
{
  SmoothMean smoother;
  smoother.set_window (4);
  
  Profile smoothed (profile);
  smoother.transform (&smoothed);

#ifdef _DEBUG
  cerr << "lower=" << last_lower << " upper=" << last_upper << endl;
#endif

  unsigned nbin = profile.get_nbin();

  for (unsigned ibin=0; ibin < nbin; ibin++) {

    float val = smoothed.get_amps()[ibin];
    weight[ibin] = val > last_lower && val < last_upper;

#ifdef _DEBUG
    cerr << "weight["<< ibin <<"]=" << weight[ibin] << " val=" << val << endl;
#endif

  }
}
