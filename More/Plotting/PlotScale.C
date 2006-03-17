/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotLabel.h"
#include "iopair.h"

using namespace std;

Pulsar::PlotScale::PlotScale () :
  range_norm (0.0, 1.0)
{
  buf_norm = 0.0;
  minval = 0.0;
  maxval = 1.0;
}

//! Set the minimum and maximum value in the data
void Pulsar::PlotScale::set_minmax (float min, float max)
{
  minval = min;
  maxval = max;
}

void 
Pulsar::PlotScale::get_range (const Archive*, float& min, float& max) const
{
  min = minval;
  max = maxval;

  stretch (range_norm, min, max);

  float space = (max - min) * buf_norm;
  min -= space;
  max += space;
}

void 
Pulsar::PlotScale::get_range (unsigned n, unsigned& imin, unsigned& imax) const
{
  float min = 0.0;
  float max = 1.0;

  // cerr << "Pulsar::PlotScale::get_range norm=" << range_norm << endl;

  stretch (range_norm, min, max);

  if (min < 0)
    min = 0;
  if (max < 0)
    max = 0;

  imin = unsigned( min * n );
  imax = unsigned( max * n );

  if (imin > n)
    imin = n;

  if (imax > n)
    imax = n;
}
