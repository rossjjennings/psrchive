/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Plot.h"
#include "Pulsar/PlotFrame.h"

bool Pulsar::Plot::verbose = false;

Pulsar::Plot::Plot ()
{
  frame = new PlotFrame;
}

//! Get the text interface to the frame attributes
TextInterface::Class* Pulsar::Plot::get_frame_interface ()
{
  return new PlotFrame::Interface (get_frame());
}

//! Get the attributes
Pulsar::PlotAttributes* Pulsar::Plot::get_attributes ()
{
  return attributes;
}

//! Get the attributes
const Pulsar::PlotAttributes* Pulsar::Plot::get_attributes () const
{
  return attributes;
}

//! Set the attributes
void Pulsar::Plot::set_attributes (PlotAttributes* a)
{
  attributes = a;
}

//! Get the frame
Pulsar::PlotFrame* Pulsar::Plot::get_frame ()
{
  return frame;
}

//! Get the frame
const Pulsar::PlotFrame* Pulsar::Plot::get_frame () const
{
  return frame;
}

//! Set the frame
void Pulsar::Plot::set_frame (PlotFrame* f)
{
  frame = f;
  attributes = f;
}

