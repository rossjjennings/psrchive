/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ChannelZapMedian.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "ModifyRestore.h"

#include "median_smooth.h"

using namespace std;

//! Default constructor
Pulsar::ChannelZapMedian::ChannelZapMedian ()
{
  cutoff_threshold = 4.0;
  window_size = 21;
  bybin = false;
}

//! Get the text interface to the configuration attributes
TextInterface::Class* Pulsar::ChannelZapMedian::get_interface ()
{
  return new Interface (this);
}

Pulsar::ChannelZapMedian::Interface::Interface (ChannelZapMedian* instance)
{
  if (instance)
    set_instance (instance);

  add( &ChannelZapMedian::get_window_size,
       &ChannelZapMedian::set_window_size,
       "window", "Size of median smoothing window" );

  add( &ChannelZapMedian::get_cutoff_threshold,
       &ChannelZapMedian::set_cutoff_threshold,
       "cutoff", "Cutoff threshold" );

  add( &ChannelZapMedian::get_bybin,
       &ChannelZapMedian::set_bybin,
       "bybin", "Run algorithm on spectra for each phase bin" );
}

void zap (vector<bool>& mask, vector<float>& spectrum,
	  unsigned window_size, float cutoff_threshold);

//! Set integration weights
void Pulsar::ChannelZapMedian::weight (Integration* integration)
{
  Reference::To<Integration> clone;

  if (bybin && !integration->get_dedispersed()) {

    /*
      RFI or other instrumental errors may produce a spike in a single
      phase bin.  Using the shift theorem of the Fourier Transform to
      rotate the phase (by a fractional bin) of a profile with a spike
      in it will convert the spike into a sinc function.  Therefore,
      ensure that phase rotations are done in the phase domain
    */
      
    ModifyRestore<bool> change (Profile::rotate_in_phase_domain, true);
    clone = integration->clone();
    clone->dedisperse();
    integration = clone;

  }

  unsigned ichan, nchan = integration->get_nchan ();
  unsigned ipol,  npol = integration->get_npol ();
  unsigned ibin,  nbin = integration->get_nbin ();

  vector<float> spectrum (nchan);
  vector<bool> mask (nchan, false);

  for (ichan=0; ichan < nchan; ichan++) {

    double polsum = 0;
    for (ipol=0; ipol<npol; ipol++) {
      double temp = integration->get_Profile (ipol, ichan) -> sum();
      polsum += temp * temp;
    }
    spectrum[ichan] = sqrt(polsum);

  }

  zap (mask, spectrum, window_size, cutoff_threshold);

  if (bybin) {

    for (ibin=0; ibin < nbin; ibin++) {

      for (ichan=0; ichan < nchan; ichan++) {

	double polsum = 0;
	for (ipol=0; ipol<npol; ipol++) {
	  Profile* profile = integration->get_Profile (ipol, ichan);
	  double temp = profile->get_amps()[ibin];
	  polsum += temp * temp;
	}
	
	spectrum[ichan] = sqrt(polsum);

      }

      zap (mask, spectrum, window_size, cutoff_threshold);

    }

  }

  unsigned total_zapped = 0;
  ostringstream oss;

  for (ichan=0; ichan < nchan; ichan++)
    if (mask[ichan]) {

      integration->set_weight (ichan, 0.0);
      
      if (total_zapped)
        oss << " ";
      oss << ichan;

      total_zapped ++;

    }

  cerr << "Pulsar::ChannelZapMedian::weight zapped " << total_zapped
       << " channels" << endl;

  cout << "Equivalent paz cmd: paz -z \"" << oss.str() << "\"" << endl;

}

void zap (vector<bool>& mask, vector<float>& spectrum,
	  unsigned window_size, float cutoff_threshold)
{
  vector<float> smoothed_spectrum = spectrum;
    
  fft::median_smooth (smoothed_spectrum, window_size);
    
  double variance = 0.0;
  unsigned total_chan = 0;

  unsigned nchan = spectrum.size();

  for (unsigned ichan=0; ichan < nchan; ichan++) {

    spectrum[ichan] -= smoothed_spectrum[ichan];

    spectrum[ichan] *= spectrum[ichan];

    if (!mask[ichan]) {
      variance += spectrum[ichan];
      total_chan ++;
    }

  }

  variance /= total_chan;

  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::ChannelZapMedian::weight variance=" << variance << endl;

  bool zapped = true;
  unsigned round = 1;

  while (zapped)  {

    float cutoff = cutoff_threshold * cutoff_threshold * variance;

    if (Pulsar::Integration::verbose)
      cerr << "Pulsar::ChannelZapMedian::weight round " << round 
	   << " cutoff=" << cutoff << endl;

    zapped = false;
    round ++;

    for (unsigned ichan=0; ichan < nchan; ichan++) {

      if (mask[ichan])
	continue;

      if (spectrum[ichan] > cutoff) {
	// cerr << "cutoff " << ichan << " " << spectrum[ichan] << endl;
	mask[ichan] = true;
      }

      if (ichan && fabs(spectrum[ichan]-spectrum[ichan-1]) > 2*cutoff) {
	// cerr << "diff " << ichan << " " << spectrum[ichan] << endl;
	mask[ichan] = true;
      }

      if (mask[ichan]) {
        variance -= spectrum[ichan]/total_chan;
        zapped = true;
      }

    }

  }

}
