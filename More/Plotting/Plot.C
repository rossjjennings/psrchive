#include "Pulsar/Plot.h"
#include "Pulsar/PlotFrameTI.h"

bool Pulsar::Plot::verbose = false;

Pulsar::Plot::Plot ()
{
  frame = new PlotFrame;
}

//! Get the text interface to the frame attributes
TextInterface::Class* Pulsar::Plot::get_frame_interface ()
{
  return new PlotFrameTI (get_frame());
}

//! Get the frame
Pulsar::PlotFrame* Pulsar::Plot::get_frame ()
{
  return frame;
}

//! Set the frame
void Pulsar::Plot::set_frame (PlotFrame* f)
{
  frame = f;
}

