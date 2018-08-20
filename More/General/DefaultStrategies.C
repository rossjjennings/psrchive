/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/Archive.h"

#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PhaseSNR.h"

#include "interface_stream.h"

using namespace Pulsar;

/*!  
  The default baseline estimator is the BaselineWindow class
*/
Pulsar::Option< Reference::To<ProfileWeightFunction> >
DefaultStrategies::default_baseline
(
 "Profile::baseline", new Pulsar::BaselineWindow,

 "Baseline estimation algorithm",

 "Configures the algorithm used to find the off-pulse baseline.\n"
 "Set equal to 'help' to see all possible configurations"
);

//! The implementation of the baseline finding algorithm
ProfileWeightFunction* DefaultStrategies::baseline () const
{
  /*
    Strategies are cloned because, in a multi-threaded application,
    each Profile instance must manage its own copy of this resource
  */

  if (!baseline_strategy)
    baseline_strategy = default_baseline.get_value()->clone();

  return baseline_strategy;
}

void DefaultStrategies::set_baseline (ProfileWeightFunction* b)
{
  baseline_strategy = b;
}


/*!  
  The default on-pulse estimator is the PeakConsecutive class
*/
Pulsar::Option< Reference::To<ProfileWeightFunction> >
DefaultStrategies::default_onpulse
(
 "Profile::onpulse", new Pulsar::PeakConsecutive,

 "On-pulse estimation algorithm",

 "Configures the algorithm used to find the on-pulse phase bins.\n"
 "Set equal to 'help' to see all possible configurations"
);

//! The implementation of the on-pulse finding algorithm
ProfileWeightFunction* DefaultStrategies::onpulse () const
{
  if (!onpulse_strategy)
    onpulse_strategy = default_onpulse.get_value()->clone();

  return onpulse_strategy;
}

void DefaultStrategies::set_onpulse (ProfileWeightFunction* pwf)
{
  onpulse_strategy = pwf;
}


/*!  The SNRatioEstimator::factory method is used to choose and
     configure the S/N estimation algorithm.
*/
Pulsar::Option< Reference::To<SNRatioEstimator> >
DefaultStrategies::default_snratio
(
 "Profile::snr", new Pulsar::PhaseSNR,

 "Algorithm used to compute S/N",

 "The name of the algorithm used to estimate the signal-to-noise ratio\n"
 "of the pulse profile. Possible values: phase, fourier, square, adaptive, \n"
 "and standard <filename.ar>"
);

//! The implementation of the signal-to-noise ratio calculator
SNRatioEstimator* DefaultStrategies::snratio () const
{
  if (!snratio_strategy)
    snratio_strategy = default_snratio.get_value()->clone();

  return snratio_strategy;
}

void DefaultStrategies::set_snratio (SNRatioEstimator* snre)
{
  snratio_strategy = snre;
}

//! Returns the strategy manager
Profile::Strategies* Profile::get_strategy() const
{
  if (!strategy)
    strategy = new DefaultStrategies;
  else
  {
    ManagedStrategies* managed
      = dynamic_cast<ManagedStrategies*>( strategy.get() );

    if (managed)
      strategy = managed->get_container()->get_strategy();
  }
  
  return strategy;
}

//! Returns the strategy manager
Profile::Strategies* Integration::get_strategy() const
{
  if (parent)
    return parent->get_strategy();

  if (orphaned)
    return orphaned->get_strategy();

  return new DefaultStrategies;
}


//! Returns the strategy manager
DefaultStrategies* Archive::get_strategy() const
{
  if (!strategy)
    strategy = new DefaultStrategies;

  return strategy;
}

Profile::Strategies* Integration::Meta::get_strategy ()
{
  if (!strategy)
    strategy = new DefaultStrategies;

  return strategy;
}

