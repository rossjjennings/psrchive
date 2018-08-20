/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/ProfileStatsInterface.h"
#include "Pulsar/ProfileStrategies.h"

#include "Pulsar/Archive.h"

#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/SNRatioEstimator.h"
#include "interface_stream.h"

using namespace Pulsar;
using namespace std;

class DefaultStrategiesInterface : public TextInterface::To<DefaultStrategies>
  {
  public:

    //! Default constructor
    DefaultStrategiesInterface ( DefaultStrategies* instance = 0 )
    {
      if (instance)
	set_instance (instance);

      add( &DefaultStrategies::onpulse,
	   &DefaultStrategies::set_onpulse,
	   &ProfileWeightFunction::get_interface,
	   "^on", "Install on-pulse estimator" );

      add( &DefaultStrategies::baseline,
	   &DefaultStrategies::set_baseline,
	   &ProfileWeightFunction::get_interface,
	   "^off", "Install off-pulse estimator" );

      add( &DefaultStrategies::snratio,
	   &DefaultStrategies::set_snratio,
	   &SNRatioEstimator::get_interface,
	   "^snr", "Install signal-to-noise ratio estimator" );

    }
    
  };

Pulsar::Statistics::Interface::Interface (Statistics* instance)
{
  if (instance)
  {
    set_instance (instance);

    if (instance->get_Archive()->get_nsubint() == 0)
      return;
  }

  add( &Statistics::get_subint,
       &Statistics::set_subint,
       "subint", "Sub-integration index" );

  add( &Statistics::get_chan,
       &Statistics::set_chan,
       "chan", "Frequency channel index" );

  add( &Statistics::get_pol,
       &Statistics::set_pol,
       "pol", "Polarization index" );

  DefaultStrategies* strategy = 0;
  if (instance)
    strategy = instance->get_strategy();

  import( DefaultStrategiesInterface(strategy), &Statistics::get_strategy );

  ProfileStats* stats = 0;
  if (instance)
    stats = instance->get_stats();

  import( ProfileStats::Interface(stats), &Statistics::get_stats );

  if (instance)
  {
    if (Archive::verbose > 2) cerr << "Pulsar::Statistics::Interface::ctor"
      " insert " << instance->plugins.size() << " plugins" << endl;

    for (unsigned i=0; i < instance->plugins.size(); i++)
      insert ( instance->plugins[i]->get_interface() );
  }

  add( &Statistics::get_effective_duty_cycle,
       "weff", "Effective pulse width (turns)" );

  add( &Statistics::get_pulse_width,
       &Statistics::set_pulse_width_estimator,
       &Statistics::get_pulse_width_interface,
       "width", "Pulse width (turns)" );

  add( &Statistics::get_snr,
       &Statistics::set_snr_estimator,
       &Statistics::get_snr_interface,
       "snr", "Signal-to-noise ratio" );

  add( &Statistics::get_nfnr,
       "nfnr", "Noise-to-Fourier-noise ratio" );

  add( &Statistics::get_cal_ntrans,
       "ncal", "Number of CAL transitions" );

  add( &Statistics::get_2bit_dist,
       "d2bit", "2-bit distortion" );

  add( &Statistics::get_weighted_frequency,
       "wtfreq", "Weighted frequency" );

  add( &Statistics::get_bin_width,
       "bwidth", "Phase bin width" );

  add( &Statistics::get_dispersive_smearing,
       "dsmear", "Dispersive smearing in worst channel" );
}
