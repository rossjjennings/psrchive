/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/LawnMower.h"
#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/SmoothMedian.h"

#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <time.h>
#include <assert.h>

using namespace std;

// #define _DEBUG 1

Pulsar::LawnMower::LawnMower ()
{
  mower = new OnPulseThreshold;

  GaussianBaseline* baseline = new GaussianBaseline;
  baseline->set_smoothing_function (0);
  baseline->set_threshold (3.0);
  baseline->set_include( include = new PhaseWeight );

  float correction = baseline->get_variance_correction ();

  mower -> set_baseline_estimator( baseline );
  mower -> set_threshold( 4.0 * correction );
  mower -> set_allow_negative (true);

  broadband = true;

  srand48 (time(0));
}

Pulsar::LawnMower::~LawnMower ()
{
}

void Pulsar::LawnMower::set_baseline_estimator (BaselineEstimator* b)
{
  mower->set_baseline_estimator (b);
}

Pulsar::BaselineEstimator* Pulsar::LawnMower::get_baseline_estimator () const
{
  return mower->get_baseline_estimator();
}

void Pulsar::LawnMower::set_threshold (float sigma)
{
  mower->set_threshold( sigma );
}

void Pulsar::LawnMower::set_broadband (bool flag)
{
  broadband = flag;
}

//! One or more preconditions can be added
void Pulsar::LawnMower::add_precondition
( Functor< bool(Profile*,PhaseWeight*) > f )
{
  precondition.push_back( f );
}

bool Pulsar::LawnMower::build_mask (Profile* profile)
{
  assert (profile != 0);

  Reference::To<Profile> smoothed = new Profile( *profile );
  SmoothMedian median;
  median.set_bins (11);
  median( smoothed );

  Reference::To<Profile> difference = new Profile( *profile );
  *difference -= *smoothed;

  /*
    The difference between the profile and its median smoothed
    difference can contain a large number of zeros (where the profile
    happens to equal the median).  These zeros cause the variance to
    be underestimated, which can mess up the IterativeBaseline
    algorithm; therefore, they should be excluded from consideration.
  */
  unsigned nbin = profile->get_nbin();
  include->resize (nbin);
  include->set_all (1.0);
  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (difference->get_amps()[ibin] == 0.0)
      (*include)[ibin] = 0;

  mower->set_Profile( difference );

  if (!mowed)
    mowed = new PhaseWeight;

  mower->get_weight( *mowed );

  unsigned total_mowed = 0;
  for (unsigned ibin=0; ibin<nbin; ibin++)
    if ( (*mowed)[ibin] )
      total_mowed ++;

  if (!total_mowed)
    return false;

  for (unsigned i=0; i<precondition.size(); i++)
    if (!precondition[i]( profile, mowed ))
      return false;

  return true;
}

void Pulsar::LawnMower::transform (Integration* subint)
{
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::LawnMower::transform" << endl;

  Reference::To<Integration> total = subint->clone();
  total->expert()->pscrunch();

  if (broadband) {

    FrequencyIntegrate integrate;
    integrate.set_dedisperse( false );

    integrate (total);

    if (!build_mask( total->get_Profile(0,0) ))
      return;

  }

  GaussianBaseline baseline;
  baseline.set_smoothing_function (0);
  float correction = baseline.get_variance_correction ();

  SmoothMedian median;
  median.set_bins (11);

  for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++) try {

#ifdef _DEBUG
    cerr << "ichan=" << ichan << "/" << subint->get_nchan() << endl;
#endif

    if (subint->get_weight(ichan) == 0)
      continue;

    if (!broadband && !build_mask( total->get_Profile (0,ichan) ))
      continue;

    Reference::To<PhaseWeight> base = 
      baseline.baseline( subint->get_Profile (0,ichan) );

    for (unsigned ipol=0; ipol < subint->get_npol(); ipol++) {

      Reference::To<Profile> profile = subint->get_Profile (ipol, ichan);

      Reference::To<Profile> smoothed = new Profile( *profile );
      median( smoothed );

      base->set_Profile( profile );

      double variance = base->get_variance().get_value();
      if (ipol == 0)
	variance *= correction;
      double rms = sqrt(variance);

#ifdef _DEBUG
      cerr << ipol << " " << ichan << " rms=" << rms
	   << " cor=" << correction << endl;
#endif

      float* amps = profile->get_amps();
      float* smamps = smoothed->get_amps();

      unsigned nbin = subint->get_nbin ();

      // 2.5 sigma should get most valid baseline samples
      for (unsigned i=0; i<nbin; i++) {

	if ( (*mowed)[i] ) {

	  unsigned count = 0;
	  unsigned ibin = 0;
	  float diff = 0;

	  do {

	    ibin = lrand48() % nbin;
	    count ++;

	    if (count == nbin)
	      throw Error (InvalidState, "Pulsar::LawnMower::transform",
			   "no baseline points available for replacement");

	    diff = amps[ibin] - smamps[ibin];

	  }
	  while ( (*mowed)[ibin] || fabs(diff) > rms * 2.5 );

	  amps[i] = smamps[i] + diff;

	}

      }
    }

  }
  catch (Error& error) {
    if (Integration::verbose)
      cerr << "Pulsar::LawnMower::transform failed on ichan=" << ichan
	   << " " << error.get_message() << endl;
    subint->set_weight (ichan, 0.0);
  }
  
}
