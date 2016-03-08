/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"

Pulsar::PhaseVsFrequency::PhaseVsFrequency ()
{
  // PhaseVsPlot gets row information from y_scale
  get_frame()->set_y_scale( new FrequencyScale );

  isubint = 0;
  ipol = 0;
}

TextInterface::Parser* Pulsar::PhaseVsFrequency::get_interface ()
{
  return new Interface (this);
}

std::string Pulsar::PhaseVsFrequency::get_ylabel (const Archive* data)
{
  return "Frequency (MHz)";
}

void Pulsar::PhaseVsFrequency::prepare (const Archive *data)
{
  PhaseVsPlot::prepare (data);
  subint = Pulsar::get_Integration (data, isubint);
}

const Pulsar::Profile*
Pulsar::PhaseVsFrequency::get_Profile (const Archive* data, unsigned ichan)
{
  return Pulsar::get_Profile (subint, ipol, ichan);
}
