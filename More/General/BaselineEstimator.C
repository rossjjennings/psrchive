/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

Pulsar::BaselineEstimator::BaselineEstimator ()
{
  median_cut = 0.0;
}

void Pulsar::BaselineEstimator::set_median_cut (float cut)
{
  median_cut = cut;
}

Pulsar::PhaseWeight*
Pulsar::BaselineEstimator::baseline (const Profile* profile)
{
  Reference::To<PhaseWeight> weight = new PhaseWeight;
  set_Profile( profile );
  get_weight( weight );

  if (median_cut) {

    float median = weight->get_median();
    float median_diff = weight->get_median_difference();

    unsigned nbin = profile->get_nbin();
    const float* amps = profile->get_amps();

    for (unsigned ibin=0; ibin<nbin; ibin++)
      if ( (*weight)[ibin] ) {
	float diff = fabs( amps[ibin] - median );
	if ( diff > median_cut * median_diff )
	  (*weight)[ibin] = 0;
      }

  }

  return weight.release();
}

Pulsar::PhaseWeight*
Pulsar::BaselineEstimator::operator () (const Profile* profile)
{
  return baseline (profile);
}

//! Include only the specified phase bins for consideration
void Pulsar::BaselineEstimator::set_include (PhaseWeight* _include)
{
  include = _include;
}

