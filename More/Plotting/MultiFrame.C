/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MultiFrame.h"

void Pulsar::MultiFrame::manage (const std::string& name, PlotFrame* frame)
{
  frames[name] = frame;
}

Pulsar::PlotFrame* Pulsar::MultiFrame::get_frame (const std::string& name)
{
  std::map< std::string, Reference::To<PlotFrame> >::iterator frame;
  frame = frames.find (name);

  if (frame == frames.end())
    throw Error (InvalidParam, "Pulsar::MultiFrame::get_frame",
		 "no frame named '" + name + "'");

  return frame->second;
}

template<typename Iterator, typename UnaryMemberFunction, typename T>
void foreach (Iterator begin, Iterator end,
	       UnaryMemberFunction func, T value)
{
  for (Iterator i = begin; i != end; i++)
    (i->second->*func)(value);
}

//! Set the character height
void Pulsar::MultiFrame::set_character_height (float height)
{
  character_height = height;
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::set_character_height, character_height );
}

//! Set the character font
void Pulsar::MultiFrame::set_character_font (int font)
{
  character_font = font;
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::set_character_font, character_font );
}

//! Set the line width
void Pulsar::MultiFrame::set_line_width (int width)
{
  line_width = width;
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::set_line_width, line_width );
}

template<typename Iterator, typename GeneratorMemberFunction>
void foreach (Iterator begin, Iterator end,
	       GeneratorMemberFunction func)
{
  for (Iterator i = begin; i != end; i++)
    (i->second->*func)();
}

void Pulsar::MultiFrame::publication_quality ()
{
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::publication_quality );
}

void Pulsar::MultiFrame::no_labels ()
{
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::no_labels );
}
