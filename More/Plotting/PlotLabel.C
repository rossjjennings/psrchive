#include "Pulsar/PlotLabel.h"

std::string Pulsar::PlotLabel::unset = "unset";

Pulsar::PlotLabel::PlotLabel ()
{
  left = centre = right = unset;
}
