/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::OnPulseThreshold::OnPulseThreshold ()
{
  threshold = 3.0; // sigma

  bin_start = bin_end = 0;
  range_specified = false;

  baseline_estimator = new BaselineWindow;
}

void Pulsar::OnPulseThreshold::set_Profile (const Profile* _profile)
{
  profile = _profile;
}

//! Set the duty cycle
void Pulsar::OnPulseThreshold::set_threshold (float _threshold)
{
  threshold = _threshold;
}

float Pulsar::OnPulseThreshold::get_threshold () const
{
  return threshold;
}

//! Set the BaselineEstimator used to find the off-pulse phase bins
void Pulsar::OnPulseThreshold::set_baseline_estimator (BaselineEstimator* b)
{
  baseline_estimator = b;
}

//! Get the BaselineEstimator used to find the off-pulse phase bins
const Pulsar::BaselineEstimator* 
Pulsar::OnPulseThreshold::get_baseline_estimator () const
{
  return baseline_estimator;
}

Pulsar::BaselineEstimator*
Pulsar::OnPulseThreshold::get_baseline_estimator ()
{
  return baseline_estimator;
}

//! Set the start and end bins of the search
void Pulsar::OnPulseThreshold::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
}

//! Retrieve the PhaseWeight
void Pulsar::OnPulseThreshold::calculate (PhaseWeight& weight)
try {

  if (!profile)
    throw Error (InvalidState, "Pulsar::OnPulseThreshold::calculate",
		 "Profile not set");

  if (!baseline_estimator)
    throw Error (InvalidState, "Pulsar::OnPulseThreshold::calculate",
		 "BaselineEstimator not set");

  baseline_estimator->set_Profile (profile);   

  PhaseWeight baseline;
  baseline_estimator->get_weight (baseline);

  Estimate<double> mean = baseline.get_mean ();
  Estimate<double> rms  = sqrt(baseline.get_variance ());

  if (Profile::verbose)
    cerr << "Pulsar::OnPulseThreshold::calculate baseline"
      " mean=" << mean << " rms=" << rms << endl;

  if ( abs(mean.get_value()) > rms.get_value() )
    cerr << "Pulsar::OnPulseThreshold::calculate WARNING baseline mean="
	 << mean.get_value() << " > rms=" << rms.get_value() << endl;

  unsigned nbin = profile->get_nbin();

  weight.resize( nbin );
  weight.set_all( 0.0 );

  int start = 0;
  int stop = nbin;

  if (range_specified) {
    start = bin_start;
    stop  = bin_end;
    nbinify (start, stop, nbin);
  }

  const float* amps = profile->get_amps();

  float cutoff = threshold * rms.get_value();

  for (unsigned ibin=start; ibin<stop; ibin++)
    if ( amps[ibin % nbin] - mean.get_value() > cutoff )
      weight[ibin] = 1.0;

}
catch (Error& error) {
  throw error += "Pulsar::OnPulseThreshold::calculate";
}
