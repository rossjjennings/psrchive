#include "Pulsar/Profile.h"

/*! 
  Fractional pulse phase window used to calculate the transitions in
  Pulsar::Profile::find_cal_transition
 */
float Pulsar::Profile::peak_edge_threshold = 0.1;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_peak_edges
//
/*!  

  Finds the edges at which the cummulative power falls below a
  threshold for the last time (rise), and where it remains above
  another threshold for the last time (fall).  The function is
  best-suited to detect the edges of a pulsar main peak.

  The threshold defining the rising edge of the pulse is defined by
  Profile::peak_edge_threshold*Profile::sum().  The falling edge
  threshold is defined by
  (1-Profile::peak_edge_threshold)*Profile::sum().  If no edges are
  found, Profile::find_max_phase is called with a duty cycle of 0.4 in
  order to find the "peak".  In this case, rise and fall are set to
  reflect the phase of peak-0.2 and peak+0.2, respectively.

  \retval rise bin at which the cummulative power last remains above threshold1
  \retval fall bin at which the cummulative power first falls below threshold2

  \post On success, this method will return rise < fall
*/
void Pulsar::Profile::find_peak_edges (int& rise, int& fall) const
{
  // the search for the edge is performed twice.
  // on the second try, the search starts half way into the profile

  // the mean is removed from each amplitude
  float min_amp = mean ( find_min_phase() );

  // the total power under the pulse
  double tot_amp = sum() - double(min_amp) * double(nbin);
  // set the thresholds for the search
  double rise_threshold = double(peak_edge_threshold) * tot_amp;
  double fall_threshold = double(1.0-peak_edge_threshold) * tot_amp;

  // space to hold the cumulative sum
  double* cumu = new double [nbin];

  // start on first bin, then on nbin/2
  int istart = 0;

  // one result for each try
  int irise[2];
  int ifall[2];  

  for (int itry=0; itry<2; itry++) {

    int ibin = 0;

    // form cumulative sum
    cumu[0] = amps[istart] - min_amp;
    for (ibin=1; ibin<nbin; ibin++)
      cumu[ibin] = cumu[ibin-1] + double(amps[(ibin+istart)%nbin] - min_amp);

    // find where cumulative sum falls below min_threshold for the last time
    irise[itry] = 0;
    for (ibin=0; ibin<nbin; ibin++)
      if (cumu[ibin] < rise_threshold) 
	irise[itry] = ibin;

    // find where cumulative sum falls below max_threshold for the first time.
    ifall[itry] = nbin-1;
    for (ibin=nbin-1; ibin>=0; ibin--) 
      if (cumu[ibin] > fall_threshold) 
	ifall[itry] = ibin;

    // do it again; this time starting half way along.
    istart = nbin/2;

  }

  delete [] cumu;

  // Decide which of the searches is better - ie. provides a narrower pulse.
  int diff0 = ifall[0] - irise[0];
  int diff1 = ifall[1] - irise[1];

  if (diff0 < diff1 && diff0 > 0) {
    rise = irise[0];
    fall = ifall[0];
  }
  else if (diff1 > 0) {
    rise = irise[1] + nbin/2;
    fall = ifall[1] + nbin/2;
  }
  else {
    // In noisy data, ifall can be < irise!  In this case, just find
    // the maximum and return with pulse width = 0.4
    float phase = find_max_phase (.4);
    rise = int ((phase - 0.2) * nbin);
    fall = int ((phase + 0.2) * nbin);
  }
}

