#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/Profile.h"

Pulsar::AdaptiveSNR::AdaptiveSNR ()
{
  initial_baseline_window = Profile::default_duty_cycle;
  baseline_threshold = 5.0;
  max_iterations = 100;
}

//! Set the width of the window used to find the initial baseline
void Pulsar::AdaptiveSNR::set_initial_baseline_window (float width)
{
  if (width < 0 || width > 1.0)
    throw Error (InvalidParam,
		 "Pulsar::AdaptiveSNR::set_initial_baseline_window",
		 "invalid width = %f periods", width);

  initial_baseline_window = width;
}

//! Set the threshold below which samples are included in the baseline
void Pulsar::AdaptiveSNR::set_baseline_threshold (float sigma)
{
  if (sigma < 0)
    throw Error (InvalidParam,
		 "Pulsar::AdaptiveSNR::set_baseline_threshold",
		 "invalid threshold = %f sigma", sigma);

  baseline_threshold = sigma;
}


//! Set the (maximum?) number of iterations
void Pulsar::AdaptiveSNR::set_max_iterations (unsigned iterations)
{
  if (iterations == 0)
    throw Error (InvalidParam,
		 "Pulsar::AdaptiveSNR::set_max_iterations",
		 "invalid iterations = %d", iterations);

  max_iterations = iterations;
}


//! Return the signal to noise ratio
float Pulsar::AdaptiveSNR::get_snr (const Profile* profile)
{
  // the mean, variance, and variance of the mean
  double mean, var, var_mean;

  if (initial_baseline_window == 1.0)
    profile->stats (&mean, &var, &var_mean);

  else {
    float centre = profile->find_min_phase(initial_baseline_window);
    profile->stats (centre, &mean, &var, &var_mean, initial_baseline_window);
  }

  if (var == 0.0)
    return 0;

  vector<bool> mask (profile->get_nbin(), false);

  const float* amps = profile->get_amps();
  unsigned nbin = profile->get_nbin();
  unsigned count = 0;

  for (unsigned iter=0; iter < max_iterations; iter++) {

    double cutoff = baseline_threshold * sqrt (var + var_mean);

    if (Profile::verbose)
      cerr << "Pulsar::AdaptiveSNR::get_snr iter=" << iter
	   << " baseline mean=" << mean << " cutoff=" << cutoff << endl;

    bool changed = false;

    double tot = 0.0;
    double totsq = 0.0;
    count = 0;

    for (unsigned ibin=0; ibin<nbin; ibin++) {

      if ( fabs(amps[ibin]-mean) < cutoff ) {

	tot += amps[ibin];
	totsq += amps[ibin]*amps[ibin];
	count ++;

	if (!mask[ibin])
	  changed = true;

	mask[ibin] = true;

      }
      else {

	if (mask[ibin])
	  changed = true;

	mask[ibin] = false;

      }

    }

    if (count < 2)
      return 0;

    mean = tot / double(count);
    double meansq = totsq / double(count);
    var = (meansq - mean*mean) * double(count)/double(count-1);
    var_mean = var / double(count);

    if (!changed)
      break;

  }

  double rms = sqrt (var);
  double energy = profile->sum() - mean * nbin;

  if (Profile::verbose)
    cerr << "Pulsar::AdaptiveSNR::get_snr " << count << " out of "
	 << nbin << " bins in baseline\n"
      "  mean=" << mean << " rms=" << rms << " energy=" << energy << " S/N=" << energy/rms << endl;

  if (energy < 0)
    energy = 0.0;

  return energy/rms;

}    

