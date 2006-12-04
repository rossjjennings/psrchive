/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotAttributes.h"

Pulsar::PlotAttributes::PlotAttributes ()
{
  character_height = 1.0;
  character_font = 1;
  line_width = 1;
}

Pulsar::PlotAttributes::~PlotAttributes ()
{
}

void Pulsar::PlotAttributes::publication_quality ()
{
  set_character_height (1.5);
  set_character_font (2);
  set_line_width (3);
}

void Pulsar::PlotAttributes::no_labels ()
{
}

//! Apply a named set of commands (a shortcut)
void Pulsar::PlotAttributes::apply_set (const std::string& name)
{
  if (name == "pub")
    publication_quality ();
  else if (name == "blank")
    no_labels ();
  else
    throw Error (InvalidParam, "Pulsar::PlotAttributes::apply_set",
		 "unrecognized set name '" + name + "'");
}
