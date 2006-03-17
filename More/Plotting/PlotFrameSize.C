/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotFrameSize.h"
#include "Pulsar/ArchiveTI.h"
#include "iopair.h"

#include <cpgplot.h>

//! Default constructor
Pulsar::PlotFrameSize::PlotFrameSize ()
  : x_range (0,1), y_range (0,1) {}

Pulsar::PlotFrameSize::PlotFrameSize (const PlotFrame* frame)
  : PlotFrame(*frame), x_range (0,1), y_range (0,1) {}

void Pulsar::PlotFrameSize::focus (const Archive* data)
{
  float x0, x1, y0, y1;
  cpgqvp (0, &x0, &x1, &y0, &y1);

  stretch (x_range, x0, x1);
  stretch (y_range, y0, y1);

  cpgsvp (x0, x1, y0, y1);

  PlotFrame::focus (data);
}
