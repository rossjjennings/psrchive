#include "Pulsar/Profile.h"

Estimate<double> 
Pulsar::PhaseGradShift (const Profile& std, const Profile& obs)
{
  Profile stdcopy = std;
  Profile prfcopy = obs;

  float snrfft = 0;
  float esnrfft = 999;

  // max float is of order 10^{38} - check that we won't exceed this
  // limiting factor in the DC term of the fourier transform

  if (stdcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "standard DC=%lf > max float", stdcopy.sum());

  if (prfcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "profile DC=%lf > max float", stdcopy.sum());

  if (Profile::verbose)
    cerr << "Profile::PhaseGradShift compare nbin="<< obs.get_nbin()
	 <<" "<< stdcopy.get_nbin() <<endl;

  if (obs.get_nbin() > stdcopy.get_nbin()) {
    if (obs.get_nbin() % stdcopy.get_nbin())
      throw Error (InvalidState, "Profile::PhaseGradShift", 
		   "profile nbin=%d standard nbin=%d",
                   obs.get_nbin(), stdcopy.get_nbin());

    unsigned nscrunch = obs.get_nbin() / stdcopy.get_nbin();
    prfcopy.bscrunch (nscrunch);
  }

  if (obs.get_nbin() < stdcopy.get_nbin()) {
    if (stdcopy.get_nbin() % obs.get_nbin())
      throw Error (InvalidState, "Profile::PhaseGradShift", 
		   "profile nbin=%d standard nbin=%d",
                   obs.get_nbin(), stdcopy.get_nbin());

    unsigned nscrunch = stdcopy.get_nbin() / obs.get_nbin();
    stdcopy.bscrunch (nscrunch);
  }

  double shift = 0.0;
  float eshift = 0.0;

  prfcopy.fftconv (stdcopy, shift, eshift, snrfft, esnrfft);
  double ephase = eshift / float(stdcopy.get_nbin());

  return Estimate<double> (shift / double(stdcopy.get_nbin()), ephase);

}


