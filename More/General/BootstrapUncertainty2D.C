/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SplineSmooth.h"
#include "BoxMuller.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace std;
using namespace Pulsar;

BootstrapUncertainty2D::BootstrapUncertainty2D ()
{
  nsample = 100;
  spline = 0;
}

void BootstrapUncertainty2D::get_uncertainty (const std::vector< std::pair<double,double> >& dat_x,
					    vector< Estimate<double> >& dat_y)
{
  assert (spline != 0);

  BoxMuller noise (time(NULL));

  unsigned ndat = dat_x.size();

  vector<double> model (ndat);
  vector<double> error (ndat);

  spline->fit (dat_x, dat_y);

  for (unsigned idat=0; idat < ndat; idat++)
  {
    model[idat] = spline->evaluate (dat_x[idat]);
    error[idat] = dat_y[idat].get_error();
  }
  
  vector< Estimate<double> > simulated = dat_y;

  vector<double> sum (ndat, 0.0);
  vector<double> sumsq (ndat, 0.0);
  
  for (unsigned ipart=0; ipart < nsample; ipart++)
  { 
    for (unsigned idat=0; idat < ndat; idat++)
      simulated[idat].val = model[idat] + noise() * error[idat];

    spline->fit (dat_x, simulated);

    for (unsigned idat=0; idat < ndat; idat++)
    {
      double y = spline->evaluate (dat_x[idat]);
      sum[idat] += y;
      sumsq[idat] += y*y;
    }
  }

  for (unsigned idat=0; idat < ndat; idat++)
  {
    double mean = sum[idat] / nsample;
    double meansq = sumsq[idat] / nsample;

    dat_y[idat].val = model[idat];
    dat_y[idat].var = meansq - mean*mean;
  }

}
