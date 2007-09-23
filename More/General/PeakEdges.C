/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PeakEdges.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>

using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::PeakEdges::PeakEdges ()
{
  threshold = 0.1; // 10% of total

  bin_start = bin_end = 0;
  range_specified = false;

  bin_rise = bin_fall = 0;
  choose = true;
}

//! Set the duty cycle
void Pulsar::PeakEdges::set_threshold (float _threshold)
{
  threshold = _threshold;
}

float Pulsar::PeakEdges::get_threshold () const
{
  return threshold;
}

//! Set the BaselineEstimator used to find the off-pulse phase bins
void Pulsar::PeakEdges::set_baseline_estimator (BaselineEstimator* b)
{
  baseline_estimator = b;
}

//! Get the BaselineEstimator used to find the off-pulse phase bins
const Pulsar::BaselineEstimator* 
Pulsar::PeakEdges::get_baseline_estimator () const
{
  return baseline_estimator;
}

Pulsar::BaselineEstimator*
Pulsar::PeakEdges::get_baseline_estimator ()
{
  return baseline_estimator;
}

//! Set the start and end bins of the search
void Pulsar::PeakEdges::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
}

//! Retrieve the PhaseWeight
void Pulsar::PeakEdges::calculate (PhaseWeight& weight) {

  compute ();

  unsigned nbin = profile->get_nbin();
  weight.resize( nbin );
  weight.set_all( 0.0 );

}

void Pulsar::PeakEdges::compute ()
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PeakEdges::compute",
		 "Profile not set");

  Reference::To<PhaseWeight> baseline;

  if (baseline_estimator) {
    baseline = new PhaseWeight;
    baseline_estimator->set_Profile (profile);
    baseline_estimator->get_weight (*baseline);
  }
  else
    baseline = profile->baseline();

  Estimate<double> mean = baseline->get_mean ();
  Estimate<double> rms  = sqrt(baseline->get_variance ());

  if (Profile::verbose)
    cerr << "Pulsar::PeakEdges::compute baseline"
      " mean=" << mean << " rms=" << rms << endl;

  if ( abs(mean.get_value()) > rms.get_value() )
    cerr << "Pulsar::PeakEdges::compute WARNING baseline mean="
	 << mean.get_value() << " > rms=" << rms.get_value() << endl;

  unsigned nbin = profile->get_nbin();

  int start = 0;
  int stop = nbin;

  if (range_specified) {
    start = bin_start;
    stop  = bin_end;
    nbinify (start, stop, nbin);
  }

  const float* amps = profile->get_amps();

  // the total power under the pulse
  double tot_amp = profile->sum() - mean.get_value() * double(nbin);

  if (Profile::verbose)
    cerr << "Pulsar::PeakEdges::compute baseline mean=" << mean.get_value()
	 << " total power=" << tot_amp << endl;

  // set the thresholds for the search
  double rise_threshold = double(threshold) * tot_amp;
  double fall_threshold = double(1.0-threshold) * tot_amp;

  // space to hold the cumulative sum
  vector<double> cumu (nbin);

  // start on first bin, then on nbin/2
  unsigned istart = 0;

  // one result for each try
  int irise[2];
  int ifall[2];  

  unsigned ntries = 2;

  if (!choose)
    ntries = 1;

  double min_amp = mean.get_value();

  for (unsigned itry=0; itry<ntries; itry++) {

    // form cumulative sum
    cumu[0] = amps[istart] - min_amp;
    for (unsigned ibin=1; ibin<nbin; ibin++)
      cumu[ibin] = cumu[ibin-1] + double(amps[(ibin+istart)%nbin] - min_amp);

    // find where cumulative sum falls below min_threshold for the last time
    irise[itry] = 0;

    int ibin = 0;
    for (ibin=start; ibin<stop; ibin++)
      if (cumu[ibin] < rise_threshold) 
	irise[itry] = ibin;

    //    cpgeras();
    //cpgsci(1);
    //cpgswin(0.0,(float)nbin,cumu[0],cumu[nbin-1]);
    //cpgmove(0.0,cumu[0]);
    //for (ibin=0;ibin<nbin;ibin++){
    //  cpgdraw((float)ibin,cumu[ibin]);
    //}
    //cpgsci(2);
    //cpgmove(0.0,rise_threshold);
    //cpgdraw((float)nbin-1,rise_threshold);
    //cpgsci(2);
    //cpgmove(0.0,fall_threshold);
    //cpgdraw((float)nbin-1,fall_threshold);

    // find where cumulative sum falls below max_threshold for the first time.
    ifall[itry] = nbin-1;
    for (ibin=stop-1; ibin>start; ibin--)
      if (cumu[ibin] > fall_threshold) 
	ifall[itry] = ibin;
    
    // Make sure cumu[0] is also tested
    if (cumu[ibin] > fall_threshold) 
      ifall[itry] = ibin;
    
    if (Profile::verbose)
      cerr << "Pulsar::PeakEdges::compute try=" << itry 
	   << " irise=" << irise[itry] << " ifall=" << ifall[itry] << endl;

    // do it again; this time starting half way along.
    istart = nbin/2;

  }

  // Decide which of the searches is better - ie. provides a narrower pulse.
  int diff0 = ifall[0] - irise[0];
  int diff1 = ifall[1] - irise[1];

  if ( !choose || (diff0 < diff1 && diff0 > 0) ) {
    bin_rise = irise[0];
    bin_fall = ifall[0];
  }
  else if (diff1 > 0) {
    bin_rise = irise[1] + nbin/2;
    bin_fall = ifall[1] + nbin/2;
  }
  else {
    // In noisy data, ifall can be < irise!  In this case, just find
    // the maximum and return with pulse width = 0.4
    float phase = profile->find_max_phase (.4);
    bin_rise = int ((phase - 0.2) * nbin);
    bin_fall = int ((phase + 0.2) * nbin);
  }

  bin_rise %= nbin;
  bin_fall %= nbin;

}


/*!
  \retval rise bin at which the cummulative power last remains above threshold
  \retval fall bin at which the cummulative power first falls below threshold
*/
void Pulsar::PeakEdges::get_indeces (int& rise, int& fall) const
{
  rise = bin_rise;
  fall = bin_fall;
}
