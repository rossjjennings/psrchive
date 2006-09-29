#include "Pulsar/PolnProfile.h"

unsigned get_last_significant (const Pulsar::PolnProfile* psd,
			       const Stokes<double>& var)
{
  unsigned n_harmonic = psd->get_nbin();
using namespace std;

  unsigned max_harmonic = 0;
  unsigned ipol, npol = 4;

  vector<float> S (n_harmonic, 0);
  double S_variance = 0;

  for (ipol=1; ipol<npol; ipol++)  {
    const float* amps = psd->get_amps(ipol);
    for (unsigned ibin=1; ibin<n_harmonic; ibin++)
      S[ibin] += amps[ibin];
    S_variance += var[ipol];
  }

  double cutoff_sigma = 3.0;

  for (ipol=0; ipol<2; ipol++) {

    const float* amps = psd->get_amps(ipol);
    if (ipol)
      amps = &(S[0]);

    double threshold = var[ipol] * cutoff_sigma * cutoff_sigma;
    if (ipol)
      threshold = S_variance * cutoff_sigma * cutoff_sigma;

    unsigned count = 0;

    // cerr << "THRESHOLD=" << threshold  << endl;

    for (unsigned ibin=1; ibin<n_harmonic; ibin++) {

      if (amps[ibin] > threshold)
	count ++;
      else
	count = 0;

      // cerr << count << " " << ibin << " " << amps[ibin]  <<endl;

      if (count > 2 && ibin > max_harmonic)
	max_harmonic = ibin;

    }
    
    // cerr << "max_harmonic = " << max_harmonic << endl;

  }

  return max_harmonic;
}
