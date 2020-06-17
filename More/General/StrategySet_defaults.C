/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"

#include "Pulsar/ProfileStats.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PhaseSNR.h"

#include "interface_stream.h"
#include "lazy.h"

using namespace Pulsar;
using namespace std;

//
// default_baseline
//

LAZY_GLOBAL(Pulsar::StrategySet, \
            Configuration::Parameter< Reference::To<ProfileWeightFunction> >,
            default_baseline, new Pulsar::BaselineWindow)

Pulsar::Option< Reference::To<ProfileWeightFunction> >
default_baseline
(
 StrategySet::get_default_baseline(),

 "Profile::baseline",

 "Baseline estimation algorithm",

 "Configures the algorithm used to find the off-pulse baseline.\n"
 "Set equal to 'help' to see all possible configurations"
);

//
// default_onpulse
//

LAZY_GLOBAL(Pulsar::StrategySet, \
            Configuration::Parameter< Reference::To<ProfileWeightFunction> >,
            default_onpulse, new Pulsar::PeakConsecutive)

Pulsar::Option< Reference::To<ProfileWeightFunction> >
default_onpulse
(
 StrategySet::get_default_onpulse(),

 "Profile::onpulse",

 "On-pulse estimation algorithm",

 "Configures the algorithm used to find the on-pulse phase bins.\n"
 "Set equal to 'help' to see all possible configurations"
);

//
// default_snratio
//

LAZY_GLOBAL(Pulsar::StrategySet, \
            Configuration::Parameter< Reference::To<SNRatioEstimator> >,
            default_snratio, new Pulsar::PhaseSNR)

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


