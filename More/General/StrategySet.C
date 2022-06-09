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
#include "Pulsar/SNRatioEstimator.h"

using namespace Pulsar;
using namespace std;

//! The implementation of the baseline finding algorithm
ProfileWeightFunction* StrategySet::baseline () const
{
  return get_stats()->get_baseline_estimator();
}

void StrategySet::set_baseline (ProfileWeightFunction* b)
{
  get_stats()->set_baseline_estimator(b);
}

//! The implementation of the on-pulse finding algorithm
ProfileWeightFunction* StrategySet::onpulse () const
{
  return get_stats()->get_onpulse_estimator();
}

void StrategySet::set_onpulse (ProfileWeightFunction* pwf)
{
  get_stats()->set_onpulse_estimator( pwf );
}

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
  if (!stats) try
  {
    stats = new ProfileStats;

    /*
      Strategies are cloned because, in a multi-threaded application,
      each Profile instance must manage its own copy of this resource
    */

    stats->set_onpulse_estimator( get_default_onpulse().get_value()->clone() );
    stats->set_baseline_estimator( get_default_baseline().get_value()->clone() );
    stats->set_snratio_estimator( get_default_snratio().get_value()->clone() );
  }
  catch (Error& error)
  {
    throw error += "StrategySet::get_stats";
  }

  return stats;
}

StrategySet::StrategySet ()
{
  // cerr << "StrategySet ctor this=" << endl;
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
  if (!strategy) try
  {
    const StrategySet::Extension* ext = get<StrategySet::Extension>();
    if (ext)
      strategy = ext->get_strategy();
    else
    {
      // cerr << "Archive::get_strategy creating new StrategySet" << endl;
      strategy = new StrategySet;
      const_cast<Archive*>(this)->add_extension( new StrategySet::Extension(strategy) );
    }
  }
  catch (Error& error)
  {
    throw error += "Archive::get_strategy";
  }

  return strategy;
}

Profile::Strategies* Integration::Meta::get_strategy ()
{
  if (!strategy)
    strategy = new StrategySet;

  return strategy;
}

void Integration::Meta::set_strategy (Profile::Strategies* _strategy)
{
  strategy = _strategy;
}

