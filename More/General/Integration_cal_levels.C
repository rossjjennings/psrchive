/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"

#include "Pulsar/Archive.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/SquareWave.h"

using namespace std;

/*! Returns the mean (and variance of the mean) off the on
  and off pulse cal levels. */
void 
Pulsar::Integration::cal_levels (vector<vector<Estimate<double> > >& high,
				 vector<vector<Estimate<double> > >& low) const
{
  SquareWave estimator;
  estimator.verbose = verbose;
  
  // Get CalInfo extension to see what cal type is
  Reference::To<const CalInfoExtension> ext;
  if (parent)
    ext = parent->get<CalInfoExtension>();

  if (ext) 
    estimator.set_nstate( ext->cal_nstate );

  estimator.levels (this, high, low);
}
