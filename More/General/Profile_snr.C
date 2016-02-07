/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/SNRatioEstimator.h"
#include "Pulsar/PhaseSNR.h"
#include "interface_stream.h"

using namespace Pulsar;
using namespace std;

/*!  The SNRatioEstimator::factory method is used to choose and
     configure the S/N estimation algorithm.
*/
Pulsar::Option< Reference::To<SNRatioEstimator> > Pulsar::Profile::snr_strategy
(
 "Profile::snr", new Pulsar::PhaseSNR,

 "Algorithm used to compute S/N",

 "The name of the algorithm used to estimate the signal-to-noise ratio\n"
 "of the pulse profile. Possible values: phase, fourier, square, adaptive, \n"
 "and standard <filename.ar>"
);

/*! This method calls Profile::snr_strategy */
float Pulsar::Profile::snr() const try {
  
  if (verbose)
    cerr << "Pulsar::Profile::snr" << endl;
  
  return snr_strategy.get_value()->get_snr (this);
  
}
catch (Error& error) {
  throw error += "Pulsar::Profile::snr";
}
