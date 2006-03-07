#include "Pulsar/PlotAxis.h"

Pulsar::PlotAxis::PlotAxis ()
{
  pgbox_opt = "BCNST";
  min_norm = 0.0;
  max_norm = 1.0;
  buf_norm = 0.05;
}
