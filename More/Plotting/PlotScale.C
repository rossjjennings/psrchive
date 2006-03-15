#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotLabel.h"
#include "iopair.h"

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
