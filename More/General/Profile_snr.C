#include <math.h>

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::snr
//

#include "Profile.h"

/*! By default, the snr is calculated using Pulsar::snr_phase */
Functor<float(const Pulsar::Profile*)> 
Pulsar::Profile::snr_functor (&snr_phase);

/*! This method calls Profile::snr_functor */
float Pulsar::Profile::snr() const try {
  
  if (verbose)
    cerr << "Pulsar::Profile::snr" << endl;
  
  return snr_functor (this);
  
}
catch (Error& error) {
  throw error += "Pulsar::Profile::snr";
}

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

/*! The default implementation of the Profile::snr method is taken
 from the old timer archive code.  Using Profile::find_min_phase and
 Profile::find_peak_edges, this function finds the integrated power in
 the pulse profile and divides this by the noise in the baseline.
*/
double Pulsar::snr_phase (const Profile* profile)
{
  // find the mean and the r.m.s. of the baseline
  double min_avg, min_var;
  profile->stats (profile->find_min_phase(), &min_avg, &min_var);
  double min_rms = sqrt (min_var);

  if (Profile::verbose)
    cerr << "Pulsar::snr_phase rms=" << min_rms << endl;

  if (min_rms == 0.0)
    return 0;

  // find the total power under the pulse
  int rise = 0, fall = 0;
  profile->find_peak_edges (rise, fall);

  double power = profile->sum (rise, fall);

  nbinify (rise, fall, profile->get_nbin());

  if (Profile::verbose)
    cerr << "Pulsar::snr_phase rise=" << rise << " fall=" << fall 
	 << " power=" << power << endl;

  // subtract the total power due to the baseline
  power -= min_avg * double (fall - rise);

  // divide by the sqrt of the number of bins
  power /= sqrt (double(fall-rise));

  return power/min_rms;
}
