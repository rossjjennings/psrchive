#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

Pulsar::PlotAxis::PlotAxis ()
{
  label = PlotLabel::unset;
  pgbox_opt = "BCNST";
  min_norm = 0.0;
  max_norm = 1.0;
  buf_norm = 0.05;
}
