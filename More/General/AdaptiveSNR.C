#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/BaselineFunction.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

//! Default constructor
Pulsar::AdaptiveSNR::AdaptiveSNR ()
{
}

//! Destructor
Pulsar::AdaptiveSNR::~AdaptiveSNR ()
{
}


//! Set the threshold below which samples are included in the baseline
void Pulsar::AdaptiveSNR::set_baseline (BaselineFunction* function)
{
  baseline = function;
}


//! Return the signal to noise ratio
float Pulsar::AdaptiveSNR::get_snr (const Profile* profile)
{
  if (!baseline)
    throw Error (InvalidState, "Pulsar::AdaptiveSNR::get_snr",
		 "no BaselineFunction provided (use set_baseline)");

  baseline->set_Profile( profile );

  PhaseWeight weight;

  baseline->get_weight (weight);

  // the mean, variance, and variance of the mean
  double mean, var, var_mean;

  weight.stats (profile, &mean, &var, &var_mean);

  double rms = sqrt (var);
  unsigned nbin = profile->get_nbin();
  double energy = profile->sum() - mean * nbin;

  double total = weight.get_weight_sum();
  double max   = weight.get_weight_max();

  double on_pulse = nbin * max - total;
  double snr = energy / sqrt(var*on_pulse);

  if (Profile::verbose) {

    cerr << "Pulsar::AdaptiveSNR::get_snr " << total << " out of " << nbin*max
	 << " bins in baseline\n  mean=" << mean << " rms=" << rms 
	 << " energy=" << energy << " S/N=" << snr << endl;

  }

  if (energy < 0)
    return 0.0;

  return snr;

}    

