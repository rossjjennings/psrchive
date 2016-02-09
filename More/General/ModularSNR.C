/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ModularSNR.h"
#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PeakConsecutive.h"

using namespace std;

//! Default constructor
Pulsar::ModularSNR::ModularSNR ()
{
  baseline_estimator = new GaussianBaseline;
  onpulse_estimator = new PeakConsecutive;
}

//! Destructor
Pulsar::ModularSNR::~ModularSNR ()
{
}

//! Set the algorithm used to identify the off-pulse baseline
void Pulsar::ModularSNR::set_baseline_estimator (ProfileWeightFunction* f)
{
  baseline_estimator = f;
}

Pulsar::ProfileWeightFunction* 
Pulsar::ModularSNR::get_baseline_estimator () const
{
  return baseline_estimator;
}

//! Set the algorithm used to identify the on-pulse region
void Pulsar::ModularSNR::set_onpulse_estimator (ProfileWeightFunction* f)
{
  onpulse_estimator = f;
}

Pulsar::ProfileWeightFunction* 
Pulsar::ModularSNR::get_onpulse_estimator () const
{
  return onpulse_estimator;
}

//! Return the signal to noise ratio
float Pulsar::ModularSNR::get_snr (const Profile* profile)
{
  Reference::To<PhaseWeight> baseline;
  baseline = baseline_estimator->operate (profile);
  unsigned off_nbin = baseline->get_nonzero_weight_count();

  Estimate<double> mean = baseline->get_mean();
  Estimate<double> variance = baseline->get_variance();

  Reference::To<PhaseWeight> onpulse;
  onpulse = onpulse_estimator->operate (profile);
  unsigned on_nbin = onpulse->get_nonzero_weight_count();

  double energy = onpulse->get_weighted_sum() - mean.val * on_nbin;

  double snr = energy / sqrt(variance.val*on_nbin);

  if (Profile::verbose)
    cerr << "Pulsar::ModularSNR::get_snr " << off_nbin << 
      " out of " << profile->get_nbin() << " bins in baseline\n"
      " mean=" << mean << " rms=" << sqrt (variance.val) <<
      " energy=" << energy << " S/N=" << snr << endl;

  if (energy < 0)
    return 0.0;

  return snr;
}    

class Pulsar::ModularSNR::Interface
  : public TextInterface::To<ModularSNR>
{
public:
  Interface (ModularSNR* instance)
  {
    if (instance)
      set_instance (instance);

    add( &ModularSNR::get_baseline_estimator,
         &ModularSNR::set_baseline_estimator,
	 &ProfileWeightFunction::get_interface,
         "off", "algorithm used to locate the off-pulse baseline" );

    add( &ModularSNR::get_onpulse_estimator,
         &ModularSNR::set_onpulse_estimator,
	 &ProfileWeightFunction::get_interface,
         "on", "algorithm used to locate the on-pulse region" );

  }

  std::string get_interface_name () const { return "modular"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ModularSNR::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::ModularSNR* Pulsar::ModularSNR::clone () const
{
  return new ModularSNR (*this);
}
