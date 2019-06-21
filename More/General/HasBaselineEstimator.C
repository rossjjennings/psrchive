/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/HasBaselineEstimator.h"
#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/GaussianBaseline.h"

using namespace std;

//! Default constructor
Pulsar::HasBaselineEstimator::HasBaselineEstimator ()
{
  baseline_estimator = new GaussianBaseline;
  other = 0;
}

Pulsar::HasBaselineEstimator::HasBaselineEstimator (const HasBaselineEstimator& copy)
{
  if (copy.baseline_estimator)
    baseline_estimator = copy.baseline_estimator->clone();
  other = 0;
}

//! Destructor
Pulsar::HasBaselineEstimator::~HasBaselineEstimator()
{
}

void Pulsar::HasBaselineEstimator::set_baseline_estimator (ProfileWeightFunction* est)
{
#if _DEBUG
    cerr << "Pulsar::HasBaselineEstimator::set_baseline_estimator this=" << this
	 << " est=" << est << endl;
#endif

  baseline_estimator = est;
  other = 0;
}

Pulsar::ProfileWeightFunction*
Pulsar::HasBaselineEstimator::get_baseline_estimator () const
{
  if (other)
    return other->baseline_estimator;

  return baseline_estimator;
}

