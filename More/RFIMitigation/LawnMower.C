/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/LawnMower.h"
#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"

#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <time.h>

using namespace std;

// #define _DEBUG 1

Pulsar::LawnMower::LawnMower ()
{
  mower = new OnPulseThreshold;

  GaussianBaseline* baseline = new GaussianBaseline;
  baseline->set_smoothing_function (0);

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
  if (!mask)
    mask = new PhaseWeight;

  mower->set_Profile( profile );
  mower->get_weight( *mask );

  for (unsigned i=0; i<precondition.size(); i++)
    if (!precondition[i]( profile, mask ))
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

  for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++) {

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

      base->set_Profile( subint->get_Profile (ipol, ichan) );

      double mean = base->get_mean().get_value();
      double variance = base->get_variance().get_value();
      if (ipol == 0)
	variance *= correction;
      double rms = sqrt(variance);

#ifdef _DEBUG
      cerr << ipol << " " << ichan << " mean=" << mean << " rms=" << rms
	   << " cor=" << correction << endl;
#endif

      float* amps = subint->get_Profile(ipol, ichan)->get_amps();
      unsigned nbin = subint->get_nbin ();

      // 2.5 sigma should get most valid baseline samples
      for (unsigned i=0; i<nbin; i++) {

	if ( (*mask)[i] ) {

	  unsigned ibin = 0;
	  do {
	    ibin = lrand48() % nbin;
	  }
	  while ( (*mask)[ibin] || fabs(amps[ibin]-mean) > rms * 2.5 );

	  amps[i] = amps[ibin];

	}

      }
    }

  }
}
