/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/ProfileStrategies.h"

#include <math.h>

using namespace std;

//! Default constructor
Pulsar::AdaptiveSNR::AdaptiveSNR ()
{
}

//! Destructor
Pulsar::AdaptiveSNR::~AdaptiveSNR ()
{
}


//! Set the threshold below which samples are included in the baseline
void Pulsar::AdaptiveSNR::set_baseline_estimator (ProfileWeightFunction* f)
{
  baseline_estimator = f;
}

Pulsar::ProfileWeightFunction* 
Pulsar::AdaptiveSNR::get_baseline_estimator () const
{
  if (baseline_estimator)
    return baseline_estimator;
  else
    return StrategySet::get_default_baseline().get_value();
}

//! Return the signal to noise ratio
float Pulsar::AdaptiveSNR::get_snr (const Profile* profile)
{
  Reference::To<PhaseWeight> baseline;

  if (baseline_estimator)
    baseline = baseline_estimator->operate (profile);
  else
    baseline = profile->baseline();

  Estimate<double> mean = baseline->get_mean();
  Estimate<double> variance = baseline->get_variance();

  unsigned nbin = profile->get_nbin();
  unsigned off_pulse = baseline->get_nonzero_weight_count();
  unsigned on_pulse = nbin - off_pulse;

  double energy = profile->sum() - mean.val * nbin;
  double snr = energy / sqrt(variance.val*on_pulse);

  if (Profile::verbose)
  {
    double rms = sqrt (variance.val);

    cerr << "Pulsar::AdaptiveSNR::get_snr " << off_pulse << " out of " << nbin
	 << " bins in baseline\n  mean=" << mean << " rms=" << rms 
	 << " energy=" << energy << " S/N=" << snr << endl;
  }

  if (energy < 0)
    return 0.0;

  return snr;
}    

class Pulsar::AdaptiveSNR::Interface
  : public TextInterface::To<AdaptiveSNR>
{
public:
  Interface (AdaptiveSNR* instance)
  {
    if (instance)
      set_instance (instance);

    add( &AdaptiveSNR::get_baseline_estimator,
         &AdaptiveSNR::set_baseline_estimator,
	 &ProfileWeightFunction::get_interface,
         "baseline", "algorithm used to locate the off-pulse baseline" );

  }

  std::string get_interface_name () const { return "adaptive"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::AdaptiveSNR::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::AdaptiveSNR* Pulsar::AdaptiveSNR::clone () const
{
  return new AdaptiveSNR (*this);
}
