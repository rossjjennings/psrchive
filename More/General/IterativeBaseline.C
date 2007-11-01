/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IterativeBaseline.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ConvertIsolated.h"

#include <math.h>

using namespace std;

// #define _DEBUG 1

Pulsar::IterativeBaseline::IterativeBaseline ()
{
  BaselineWindow* window = new BaselineWindow;
  window->set_duty_cycle (Profile::default_duty_cycle);

  set_initial_baseline (window);
  set_smoothing_function (new ConvertIsolated);

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

void Pulsar::IterativeBaseline::set_smoothing_function (PhaseWeightSmooth* s)
{
  smoothing_function = s;
}

Pulsar::PhaseWeightSmooth*
Pulsar::IterativeBaseline::get_smoothing_function () const
{
  return smoothing_function;
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


void Pulsar::IterativeBaseline::calculate (PhaseWeight& weight)
{
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::IterativeBaseline::calculate" << endl;

  if (!profile)
    throw Error (InvalidState, "Pulsar::IterativeBaseline::calculate",
		 "no Profile supplied (use set_Profile)");

  bool  drift_detected = false;
  float drift_threshold = 0.0;

  if (initial_baseline) try {

#ifndef _DEBUG
    if (Profile::verbose)
#endif
      cerr << "Pulsar::IterativeBaseline::calculate initial_baseline" << endl;

    initial_baseline->set_Profile (profile);
    initial_baseline->get_weight (weight);

    if (include) {
#ifndef _DEBUG
    if (Profile::verbose)
#endif
      cerr << "Pulsar::IterativeBaseline::calculate mask include" << endl;
      weight *= *include;
    }

    Estimate<double> initial_mean = weight.get_mean();
    Estimate<double> initial_rms  = sqrt( weight.get_variance() );

#ifndef _DEBUG
    if (Profile::verbose)
#endif
    cerr << "initial mean=" << initial_mean << " rms=" << initial_rms << endl;

    drift_threshold = initial_mean.get_value() + initial_rms.get_value();

    initial_bounds = true;

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

    initial_bounds = false;

    if (lower == upper)
      throw Error (InvalidState, "Pulsar::IterativeBaseline::get_weight",
		   "lower bound equals upper bound = %f", upper);

    unsigned added = 0;
    unsigned subtracted = 0;

    for (unsigned ibin=0; ibin<nbin; ibin++) {

      if ( amps[ibin] > lower && amps[ibin] < upper )  {
	
	if (weight[ibin] == 0.0 && (!include || (*include)[ibin])) {
	  added ++;
#ifdef _DEBUG
	  cerr << "+ ibin=" << ibin << " v=" << amps[ibin] << endl;
#endif
	}
	
	weight[ibin] = 1.0;
	
      }
      else {
	
	if (weight[ibin] == 1.0 && (!include || (*include)[ibin])) {
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

    if (drift_detected)
      break;

    if (drift_threshold != 0.0 && weight.get_mean() > drift_threshold) {

#ifdef _DEBUG
      cerr << "drift detected" << endl;
#endif

      drift_detected = true;
      initial_baseline->get_weight (weight);
    }

  }

  if (iter >= max_iterations)
    throw Error (InvalidState, "Pulsar::IterativeBaseline::get_weight",
		 "did not converge in %d iterations", max_iterations);

  postprocess (weight, *profile);

#ifdef _DEBUG
  cerr << "after postprocessing, mean=" << weight.get_mean() << endl;
#endif

  if (smoothing_function) {
    smoothing_function->set_weight( &weight );
    smoothing_function->get_weight( weight );
  }

#ifdef _DEBUG
  cerr << "after smoothing, mean=" << weight.get_mean() << endl;

  unsigned total = 0;
  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (weight[ibin])
      total ++;
  cerr << "total bins = " << total << endl;
#endif

}
