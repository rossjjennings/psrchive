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

  float minphs = pcopy->find_min_phase(0.6);
  *pcopy -= (pcopy->mean(minphs));

  minphs = scopy->find_min_phase(0.6);
  *scopy -= (scopy->mean(minphs));

  double shift = 0.0;
  double scale = 0.0;

  Reference::To<Pulsar::Profile> diff = 
    pcopy->morphological_difference(*scopy, scale, shift);

  pcopy->rotate(shift);

  double mean    = 0.0;
  double var     = 0.0;
  double varmean = 0.0;

  // Find the statistics of the difference profile

  diff->stats(&mean, &var, &varmean);
  double stddev1 = sqrt(var);

  // Now it gets interesting...

  int rise, fall;

  scopy->find_peak_edges(rise, fall);

  if (fall <= rise)
    fall += ( (rise-fall)/scopy->get_nbin() + 1 ) * scopy->get_nbin();

  double puls_flux = pcopy->sum(rise, fall)*scale - mean * double(fall-rise);

  // divide by the sqrt of the number of bins
  puls_flux /= sqrt (double(fall-rise));

  return (puls_flux / stddev1);
}



