/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStrategies.h"
#include "Pulsar/SNRatioEstimator.h"

using namespace Pulsar;
using namespace std;

/*! This method calls Profile::snr_strategy */
float Pulsar::Profile::snr() const try {
  
  if (verbose)
    cerr << "Pulsar::Profile::snr" << endl;
  
  return get_strategy()->snratio()->get_snr (this);
  
}
catch (Error& error) {
  throw error += "Pulsar::Profile::snr";
}
