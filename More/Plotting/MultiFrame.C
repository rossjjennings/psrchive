#include "Pulsar/MultiFrame.h"

Pulsar::PlotFrameSize* 
Pulsar::MultiFrame::manage (const std::string& name, PlotFrame* frame)
{
  Reference::To<PlotFrameSize> sized = new PlotFrameSize (frame);
  frames[name] = sized;
  return sized;
}

Pulsar::PlotFrameSize* Pulsar::MultiFrame::get_frame (const std::string& name)
{
  return frames[name];
}
