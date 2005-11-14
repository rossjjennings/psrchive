#include "Pulsar/IterativeBaseline.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <math.h>

// #define _DEBUG 1

Pulsar::IterativeBaseline::IterativeBaseline ()
{
  BaselineWindow* window = new BaselineWindow;
  window->set_duty_cycle (Profile::default_duty_cycle);

  set_initial_baseline (window);

  threshold = 2.0;
  max_iterations = 100;
}

Pulsar::IterativeBaseline::~IterativeBaseline ()
{
}

void Pulsar::IterativeBaseline::set_initial_baseline (BaselineEstimator* b)
{
  initial_baseline = b;
}

Pulsar::BaselineEstimator* 
Pulsar::IterativeBaseline::get_initial_baseline () const
{
  return initial_baseline;
}

void Pulsar::IterativeBaseline::set_threshold (float sigma)
{
  if (sigma < 0)
    throw Error (InvalidParam,
		 "Pulsar::IterativeBaseline::set_threshold",
		 "invalid threshold = %f sigma", sigma);

  threshold = sigma;
}

void Pulsar::IterativeBaseline::set_max_iterations (unsigned iterations)
{
  if (iterations == 0)
    throw Error (InvalidParam,
		 "Pulsar::IterativeBaseline::set_max_iterations",
		 "invalid iterations = %d", iterations);

  max_iterations = iterations;
}


void Pulsar::IterativeBaseline::set_Profile (const Profile* _profile)
{
  profile = _profile;
}


void Pulsar::IterativeBaseline::get_weight (PhaseWeight& weight)
{
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::IterativeBaseline::get_weight" << endl;

  if (!profile)
    throw Error (InvalidState, "Pulsar::IterativeBaseline::get_weight",
		 "no Profile supplied (use set_Profile)");

  // the mean, variance, and variance of the mean
  double mean, var, var_mean;

  if (initial_baseline) try {

#ifndef _DEBUG
    if (Profile::verbose)
#endif
      cerr << "Pulsar::IterativeBaseline::get_weight initial_baseline" << endl;

    initial_baseline->set_Profile (profile);
    initial_baseline->get_weight (weight);

  }
  catch (Error& error) {
    throw error += "Pulsar::IterativeBaseline::get_weight";
  }

  const float* amps = profile->get_amps();
  unsigned nbin = profile->get_nbin();

  unsigned iter = 0;

  float lower, upper;

  for (iter=0; iter < max_iterations; iter++) {

    get_bounds (weight, lower, upper);

    if (lower == upper) {
#ifndef _DEBUG
      if (Profile::verbose)
#endif
	cerr << "Pulsar::IterativeBaseline::get_weight "
	  "lower bound equals upper bound = " << upper << endl;

      break;
    }

    unsigned added = 0;
    unsigned subtracted = 0;

    for (unsigned ibin=0; ibin<nbin; ibin++) {

      if ( amps[ibin] > lower && amps[ibin] < upper )  {
	
	if (weight[ibin] == 0.0) {
	  added ++;
#ifdef _DEBUG
	  cerr << "+ ibin=" << ibin << " v=" << amps[ibin] << endl;
#endif
	}
	
	weight[ibin] = 1.0;
	
      }
      else {
	
	if (weight[ibin] == 1.0) {
	  subtracted ++;
#ifdef _DEBUG
	  cerr << "- ibin=" << ibin << " v=" << amps[ibin] << endl;
#endif
	}
	
	weight[ibin] = 0.0;
	
      }
      
    }

#ifdef _DEBUG
    cerr << "added=" << added << " subtracted=" << subtracted << endl;
#endif

    if (!added && !subtracted) {
#ifdef _DEBUG
      cerr << "no change" << endl;
#endif
      break;
    }

  }

  if (iter >= max_iterations)
    throw Error (InvalidState, "Pulsar::IterativeBaseline::get_weight",
		 "did not converge in %d iterations", max_iterations);

  // fill in the blank spots (otherwise, will underestimate variance)
  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (!weight[ibin] && weight[(ibin+1)%nbin]) {

#ifdef _DEBUG
      cerr << "x ibin=" << ibin << " v=" << amps[ibin] << endl;
#endif

      weight[ibin] = 1.0;

    }


}
