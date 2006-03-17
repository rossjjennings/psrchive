/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <math.h>

#include "Pulsar/FourierSNR.h"
#include "Pulsar/Profile.h"

#include "fftm.h"

Pulsar::FourierSNR::FourierSNR ()
{
  baseline_extent = 0.45;
}

//! Set the fractional number of high frequencies used to calculate noise
void Pulsar::FourierSNR::set_baseline_extent (float extent)
{
  if (extent < 0 || extent > 1.0)
    throw Error (InvalidParam, "Pulsar::FourierSNR::set_baseline_extent",
		 "invalid extent = %f", extent);

  baseline_extent = extent;
}

//! Return the signal to noise ratio
float Pulsar::FourierSNR::get_snr (const Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = new float [nbin + 2];

  fft::frc1d (nbin, amps, profile->get_amps());

  // note that DC and nyquist terms are thrown out in the following loop
  unsigned noise_start = unsigned (nbin * (1.0 - baseline_extent));
  unsigned noise_count = 0;
  unsigned total_count = 0;

  double total_power = 0.0;
  double noise_power = 0.0;

  for (unsigned ibin=2; ibin < nbin; ibin+=2) {

    double amp = sqrt( amps[ibin]*amps[ibin] + amps[ibin+1]*amps[ibin+1] );
    total_power += amp;
    total_count ++;

    if (ibin > noise_start) {
      noise_power += amp;
      noise_count ++;
    }

  }

  delete amps;

  double mean_noise = noise_power/noise_count;

  double pulse_power = total_power - mean_noise * total_count;

  if (Profile::verbose)
    cerr << "Pulsar::FourierSNR::get_snr power\n"
      "  total=" << total_power << endl <<
      "  mean_noise=" << mean_noise << endl <<
      "  pulse=" << pulse_power << endl;

  if (pulse_power <= 0)
    return 0;

  double snr = sqrt(pulse_power/mean_noise);

  if (Profile::verbose)
    cerr << "Pulsar::FourierSNR::get_snr S/N=" << snr << endl;

  return snr;

}    

