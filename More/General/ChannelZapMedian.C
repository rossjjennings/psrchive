/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ChannelZapMedian.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "median_smooth.h"

//! Default constructor
Pulsar::ChannelZapMedian::ChannelZapMedian ()
{
  cutoff_threshold = 4.0;
  window_size = 21;
}

//! Set integration weights
void Pulsar::ChannelZapMedian::weight (Integration* integration)
{
  unsigned ichan, nchan = integration->get_nchan ();

  vector<float> spectrum (nchan);

  for (ichan=0; ichan < nchan; ichan++) {
    spectrum[ichan] = integration->get_Profile (0, ichan) -> sum();
    if (integration->get_state() == Signal::PPQQ ||
	integration->get_state() == Signal::Coherence)
      spectrum[ichan] += integration->get_Profile (1, ichan) -> sum();
  }

  vector<float> smoothed_spectrum = spectrum;

  fft::median_smooth (smoothed_spectrum, window_size);

  double variance = 0.0;
  for (ichan=0; ichan < nchan; ichan++) {

    //cerr << ichan << ":" << spectrum[ichan] << ":"
    // << smoothed_spectrum[ichan] << endl;

    spectrum[ichan] -= smoothed_spectrum[ichan];
    spectrum[ichan] *= spectrum[ichan];

    variance += spectrum[ichan];
  }

  variance /= nchan;

  vector<bool> mask (nchan, false);

  bool zapped = true;
  unsigned round = 1;
  unsigned total_zapped = 0;

  while (zapped)  {

    float cutoff = cutoff_threshold * cutoff_threshold * variance;

    if (Integration::verbose)
      cerr << "Pulsar::ChannelZapMedian::weight round " << round 
	   << " cutoff=" << cutoff << endl;

    zapped = false;
    round ++;

    for (ichan=0; ichan < nchan; ichan++) {

      if (mask[ichan])
	continue;

      if (spectrum[ichan] > cutoff) {
	//cerr << "cutoff" << endl;
	mask[ichan] = true;
      }

      if (ichan && fabs(spectrum[ichan]-spectrum[ichan-1]) > 2*cutoff) {
	//cerr << "diff" << endl;
	mask[ichan] = true;
      }

      if (mask[ichan]) {
        variance -= spectrum[ichan]/nchan;
        total_zapped ++;
        zapped = true;
      }

    }

  }

  cerr << "Pulsar::ChannelZapMedian::weight zapped " << total_zapped
       << " channels" << endl;

  for (ichan=0; ichan < nchan; ichan++)
    if (mask[ichan])
      integration->set_weight (ichan, 0.0);
}
