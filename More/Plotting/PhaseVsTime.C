/***************************************************************************
 *
 *   Copyright (C) 2006 - 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/TimeScale.h"

#include "Pulsar/Archive.h"

Pulsar::PhaseVsTime::PhaseVsTime ()
{
  // PhaseVsPlot gets row information from y_scale
  get_frame()->set_y_scale( y_scale = new TimeScale );

  ichan = 0;
  ipol = 0;
}

TextInterface::Parser* Pulsar::PhaseVsTime::get_interface ()
{
  return new Interface (this);
}

const Pulsar::Profile*
Pulsar::PhaseVsTime::get_Profile (const Archive* data, unsigned isubint)
{
  return Pulsar::get_Profile (data, isubint, ipol, ichan);
}
