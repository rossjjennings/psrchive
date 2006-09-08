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

void Pulsar::PlotAttributes::set_publication_quality (bool flag)
{
  if (flag) {
    set_character_height (1.5);
    set_character_font (2);
    set_line_width (3);
  }
  else {
    set_character_height (1);
    set_character_font (1);
    set_line_width (1);
  }
}
