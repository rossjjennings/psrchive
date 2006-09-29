/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Profile.h"
#include "model_profile.h"

void Pulsar::Profile::fftconv (const Profile& std, 
			       double& shift, float& eshift, 
			       float& snrfft, float& esnrfft) const
{
  double scale, sigma_scale;
  double dshift, sigma_dshift;
  double chisq;
  
  int ret=0;

  if (!Profile::legacy)
    ret = model_profile (nbin, 1, &amps, &(std.amps), &scale, &sigma_scale, 
			   &dshift, &sigma_dshift, &chisq, verbose);
  else
    ret = legacy_fftconv (nbin, &amps[0], &(std.amps[0]),
			  &dshift, &sigma_dshift, &snrfft, &esnrfft);
  
  if (ret != 0)
    throw Error (FailedCall, "Profile::fftconv", "model_profile failed");
  if (verbose) cerr << "Profile::fftconv"
		 " shift=" << dshift << " bins,"
		 " error=" << sigma_dshift <<
		 " scale=" << scale << 
		 " error=" << sigma_scale <<
		 " chisq=" << chisq << endl;

  double rms = sqrt( chisq / (float(nbin/2)-1.0) );

  shift = dshift;

  eshift = sigma_dshift;
  if (!Pulsar::Profile::legacy) {
    snrfft = 2 * sqrt( float(nbin) ) * scale / rms;
    
    esnrfft = snrfft * eshift / scale;
  }
}
