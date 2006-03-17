/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotLabel.h"

std::string Pulsar::PlotLabel::unset = "unset";

Pulsar::PlotLabel::PlotLabel ()
{
  left = centre = right = unset;
}
