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

float Pulsar::StandardSNR::get_morph_snr (const Profile* profile)
{
  if (Pulsar::Profile::verbose)
    cerr << "Pulsar::StandardSNR::get_morph_snr" << endl;

  Reference::To<Pulsar::Profile> pcopy = profile->clone();
  Reference::To<Pulsar::Profile> scopy = standard->clone();

  if (pcopy->get_nbin() > scopy->get_nbin()) {
    pcopy->bscrunch(pcopy->get_nbin() / scopy->get_nbin());
  }
  if (pcopy->get_nbin() < scopy->get_nbin()) {
    scopy->bscrunch(scopy->get_nbin() / pcopy->get_nbin());
  }

  Reference::To<Pulsar::Profile> diff = 
    pcopy->morphological_difference(*scopy);

  double mean    = 0.0;
  double var     = 0.0;
  double varmean = 0.0;

  // Find the statistics of the difference profile

  diff->stats(&mean, &var, &varmean);
  double stddev1 = sqrt(var);

  // Now it gets interesting... How exactly is S/N defined?

  double base_flux = pcopy->get_nbin() * stddev1;
  double puls_flux = pcopy->sum();

  if (base_flux < 0.0000001)
    return 1000000.0;

  return (puls_flux / base_flux);
}



