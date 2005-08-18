#include <math.h>

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

  weight.set_Profile (profile);

  Estimate<double> mean = weight.get_mean();
  Estimate<double> variance = weight.get_variance();

  double rms = sqrt (variance.val);
  unsigned nbin = profile->get_nbin();
  double energy = profile->sum() - mean.val * nbin;

  double total = weight.get_weight_sum();
  double max   = weight.get_weight_max();

  double on_pulse = nbin * max - total;
  double snr = energy / sqrt(variance.val*on_pulse);

  if (Profile::verbose) {

    cerr << "Pulsar::AdaptiveSNR::get_snr " << total << " out of " << nbin*max
	 << " bins in baseline\n  mean=" << mean << " rms=" << rms 
	 << " energy=" << energy << " S/N=" << snr << endl;

  }

  if (energy < 0)
    return 0.0;

  return snr;

}    

