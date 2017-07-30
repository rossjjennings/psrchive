/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/SquareWave.h"

using namespace std;

/*! Returns the mean (and variance of the mean) off the on
  and off pulse cal levels. */
void 
Pulsar::Integration::cal_levels (vector<vector<Estimate<double> > >& high,
				 vector<vector<Estimate<double> > >& low) const
{
  if (verbose)
    cerr << "Pulsar::Integration::cal_levels using SquareWave::levels" << endl;
  
  SquareWave estimator;
  estimator.verbose = verbose;
  estimator.levels (this, high, low);
}
