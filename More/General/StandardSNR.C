#include "Pulsar/StandardSNR.h"
#include "Pulsar/Profile.h"

void Pulsar::StandardSNR::set_standard (const Profile* profile)
{
  standard = profile;
}

float Pulsar::StandardSNR::get_snr (const Profile* profile)
{
  if (Pulsar::Profile::verbose)
    cerr << "Pulsar::StandardSNR::get_snr" << endl;

  float ephase, snrfft, esnrfft;

  try {
    profile->shift (*standard, ephase, snrfft, esnrfft);
  }
  catch (...) {
    return 0.0;
  }

  return snrfft;
}    

