#include "Pulsar/Profile.h"
#include "Error.h"

#include "model_profile.h"

/*!
  Calculates the shift between
  Returns a basic Tempo::toa object
*/
Tempo::toa Pulsar::Profile::toa (const Profile& std, const MJD& mjd, 
				 double period, char nsite) const
{
  Tempo::toa retval;

  float ephase, snrfft, esnrfft; 
  double phase = shift (std, ephase, snrfft, esnrfft);

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + phase * period);
  retval.set_error     (ephase * period * 1e6);

  retval.set_telescope (nsite);

  if (verbose) {
    fprintf (stderr, "Pulsar::Profile::toa created:\n");
    retval.unload (stderr);
  }
  return retval;
}


double Pulsar::Profile::shift (const Profile& std, float& ephase,
			       float& snrfft, float& esnrfft) const 
{
  Profile stdcopy = std;
  Profile prfcopy = *this;

  // set these in case something goes wrong
  ephase = 999;
  snrfft = 0;
  esnrfft = 999;

  // max float is of order 10^{38} - check that we won't exceed this
  // limiting factor in the DC term of the fourier transform

  if (stdcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::shift", 
		 "standard DC=%lf > max float", stdcopy.sum());

  if (prfcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::shift", 
		 "profile DC=%lf > max float", stdcopy.sum());

  if (verbose)
    cerr << "Profile::shift compare nbin="<< nbin <<" "<< stdcopy.nbin <<endl;

  if (nbin > stdcopy.nbin) {
    if (nbin % stdcopy.nbin)
      throw Error (InvalidState, "Profile::shift", 
		   "profile nbin=%d standard nbin=%d", nbin, stdcopy.nbin);

    unsigned nscrunch = nbin / stdcopy.nbin;
    prfcopy.bscrunch (nscrunch);
  }

  if (nbin < stdcopy.nbin) {
    if (stdcopy.nbin % nbin)
      throw Error (InvalidState, "Profile::shift", 
		   "profile nbin=%d standard nbin=%d", nbin, stdcopy.nbin);

    unsigned nscrunch = stdcopy.nbin / nbin;
    stdcopy.bscrunch (nscrunch);
  }

  double shift = 0.0;
  float eshift = 0.0;

  prfcopy.fftconv (stdcopy, shift, eshift, snrfft, esnrfft);

  if (ephase)
    ephase = eshift / float(stdcopy.nbin);

  return shift / double(stdcopy.nbin);

}


void Pulsar::Profile::fftconv (Profile& std, 
			       double& shift, float& eshift, 
			       float& snrfft, float& esnrfft)
{
  double scale, sigma_scale;
  double dshift, sigma_dshift;
  double chisq;
  
  int ret = model_profile (nbin, 1, &amps, &(std.amps), &scale, &sigma_scale, 
			   &dshift, &sigma_dshift, &chisq, verbose);

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

  snrfft = 2 * sqrt( float(nbin) ) * scale / rms;

  esnrfft = snrfft * eshift / scale;
}  

