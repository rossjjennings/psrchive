/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/HasBaselineEstimator.h"
#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/GaussianBaseline.h"
#include "debug.h"
using namespace std;

//! Default constructor
Pulsar::HasBaselineEstimator::HasBaselineEstimator ()
{
  baseline_estimator = new GaussianBaseline;
}

Pulsar::HasBaselineEstimator::HasBaselineEstimator (const HasBaselineEstimator& copy)
{
  if (copy.baseline_estimator)
    baseline_estimator = copy.baseline_estimator->clone();
}

Pulsar::HasBaselineEstimator::~HasBaselineEstimator()
{
  DEBUG("Pulsar::HasBaselineEstimator dtor this=" << this << " other=" << other);
}

void Pulsar::HasBaselineEstimator::set_baseline_estimator (ProfileWeightFunction* est)
{
  DEBUG("Pulsar::HasBaselineEstimator::set_baseline_estimator this=" << this << " est=" << est);
  baseline_estimator = est;
}

Pulsar::ProfileWeightFunction*
Pulsar::HasBaselineEstimator::get_baseline_estimator () const
{
  return baseline_estimator;
}

bool Pulsar::HasBaselineEstimator::has_baseline_estimator () const
{
  return baseline_estimator;
}
