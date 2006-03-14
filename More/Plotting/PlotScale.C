#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotLabel.h"
#include "iopair.h"

Pulsar::PlotScale::PlotScale () :
  range_norm (0.0, 1.0)
{
  buf_norm = 0.0;
}

void Pulsar::PlotScale::get_range (float& min, float& max) const
{
  stretch (range_norm, min, max);

  float space = (max - min) * buf_norm;
  min -= space;
  max += space;
}
