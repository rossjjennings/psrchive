/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
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
  std::map< std::string, Reference::To<PlotFrameSize> >::iterator frame;
  frame = frames.find (name);

  if (frame == frames.end())
    throw Error (InvalidParam, "Pulsar::MultiFrame::get_frame",
		 "no frame named '" + name + "'");

  return frame->second;
}
