/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/ArchiveExtension.h"

#include "Pulsar/ProfileStats.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PhaseSNR.h"

#include "interface_stream.h"
#include "lazy.h"

using namespace Pulsar;
using namespace std;

LAZY_GLOBAL(Pulsar::StrategySet, \
            Configuration::Parameter< Reference::To<ProfileWeightFunction> >,
            default_baseline, new Pulsar::BaselineWindow)

/*!  
  The default baseline estimator is the BaselineWindow class
*/
Pulsar::Option< Reference::To<ProfileWeightFunction> >
default_baseline
(
 StrategySet::get_default_baseline(),

 "Profile::baseline",

 "Baseline estimation algorithm",

 "Configures the algorithm used to find the off-pulse baseline.\n"
 "Set equal to 'help' to see all possible configurations"
);

//! The implementation of the baseline finding algorithm
ProfileWeightFunction* StrategySet::baseline () const
{
  return get_stats()->get_baseline_estimator();
}

void StrategySet::set_baseline (ProfileWeightFunction* b)
{
  get_stats()->set_baseline_estimator(b);
}

LAZY_GLOBAL(Pulsar::StrategySet, \
            Configuration::Parameter< Reference::To<ProfileWeightFunction> >,
            default_onpulse, new Pulsar::PeakConsecutive)

/*!  
  The default on-pulse estimator is the PeakConsecutive class
*/
Pulsar::Option< Reference::To<ProfileWeightFunction> >
default_onpulse
(
 StrategySet::get_default_onpulse(),

 "Profile::onpulse",

 "On-pulse estimation algorithm",

 "Configures the algorithm used to find the on-pulse phase bins.\n"
 "Set equal to 'help' to see all possible configurations"
);

//! The implementation of the on-pulse finding algorithm
ProfileWeightFunction* StrategySet::onpulse () const
{
  return get_stats()->get_onpulse_estimator();
}

void StrategySet::set_onpulse (ProfileWeightFunction* pwf)
{
  get_stats()->set_onpulse_estimator( pwf );
}

LAZY_GLOBAL(Pulsar::StrategySet, \
            Configuration::Parameter< Reference::To<SNRatioEstimator> >,
            default_snratio, new Pulsar::PhaseSNR)

/*!  The SNRatioEstimator::factory method is used to choose and
     configure the S/N estimation algorithm.
*/
Pulsar::Option< Reference::To<SNRatioEstimator> >
default_snratio
(
 StrategySet::get_default_snratio(),

 "Profile::snr",

 "Algorithm used to compute S/N",

 "The name of the algorithm used to estimate the signal-to-noise ratio\n"
 "of the pulse profile. Possible values: phase, fourier, square, adaptive, \n"
 "and standard <filename.ar>"
);

//! The implementation of the signal-to-noise ratio calculator
SNRatioEstimator* StrategySet::snratio () const
{
  return get_stats()->get_snratio_estimator();
}

void StrategySet::set_snratio (SNRatioEstimator* snre)
{
  get_stats()->set_snratio_estimator( snre );
}

//! The implementation of the pulse width estimator
WidthEstimator* StrategySet::width () const
{
  return get_stats()->get_width_estimator();
}

void StrategySet::set_width (WidthEstimator* we)
{
  get_stats()->set_width_estimator( we );
}

ProfileStats* StrategySet::get_stats () const
{
  if (!stats)
  {
    stats = new ProfileStats;

    /*
      Strategies are cloned because, in a multi-threaded application,
      each Profile instance must manage its own copy of this resource
    */

    stats -> set_onpulse_estimator( default_onpulse.get_value()->clone() );
    stats -> set_baseline_estimator( default_baseline.get_value()->clone() );
    stats -> set_snratio_estimator( default_snratio.get_value()->clone() );
  }

  return stats;
}

StrategySet::StrategySet ()
{
}

StrategySet::StrategySet (const StrategySet& copy)
{
  if (copy.stats)
    stats = copy.stats->clone();
}

StrategySet* StrategySet::clone () const
{
  return new StrategySet (*this);
}

//! Returns the strategy manager
Profile::Strategies* Profile::get_strategy() const
{
  if (!strategy)
    strategy = new StrategySet;
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

  return new StrategySet;
}

class StrategySet::Extension : public Archive::Extension
{
  Reference::To<StrategySet> set;

  public:

    //! Default constructor
    Extension (StrategySet* _set) : Archive::Extension("StrategySet") { set = _set; }

    //! Copy constructor
    Extension (const Extension& copy) : Archive::Extension("StrategySet") { set = copy.set->clone(); }

    //! Clone method
    Extension* clone () const { return new Extension ( *this ); }

    StrategySet* get_strategy() const { return set; }
};


//! Returns the strategy manager
StrategySet* Archive::get_strategy() const
{
  if (!strategy)
  {
    const StrategySet::Extension* ext = get<StrategySet::Extension>();
    if (ext)
      strategy = ext->get_strategy();
    else
    {
      strategy = new StrategySet;
      const_cast<Archive*>(this)->add_extension( new StrategySet::Extension(strategy) );
    }
  }

  return strategy;
}

Profile::Strategies* Integration::Meta::get_strategy ()
{
  if (!strategy)
    strategy = new StrategySet;

  return strategy;
}

