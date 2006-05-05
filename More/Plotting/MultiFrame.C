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

template<typename Iterator, typename UnaryMemberFunction, typename T>
void for_each (Iterator begin, Iterator end,
	       UnaryMemberFunction func, T value)
{
  for (Iterator i = begin; i != end; i++)
    (i->second->*func)(value);
}

//! Set the character height
void Pulsar::MultiFrame::set_character_height (float height)
{
  character_height = height;
  for_each( frames.begin(), frames.end(), 
	    &PlotFrame::set_character_height, character_height );
}

//! Set the character font
void Pulsar::MultiFrame::set_character_font (int font)
{
  character_font = font;
  for_each( frames.begin(), frames.end(), 
	    &PlotFrame::set_character_font, character_font );
}

//! Set the line width
void Pulsar::MultiFrame::set_line_width (int width)
{
  line_width = width;
  for_each( frames.begin(), frames.end(), 
	    &PlotFrame::set_line_width, line_width );
}

void Pulsar::MultiFrame::set_publication_quality (bool flag)
{
  for_each( frames.begin(), frames.end(), 
	    &PlotFrame::set_publication_quality, flag );
}
