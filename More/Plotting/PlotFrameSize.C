#include "Pulsar/PlotFrameSize.h"
#include "Pulsar/ArchiveTI.h"

//! Default constructor
Pulsar::PlotFrameSize::PlotFrameSize ()
  : bottom_left (0,0), top_right (1,1) {}

Pulsar::PlotFrameSize::PlotFrameSize (const PlotFrame* frame)
  : PlotFrame(*frame), bottom_left (0,0), top_right (1,1) {}

