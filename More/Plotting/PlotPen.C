/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotPen.h"

#include <cpgplot.h>

//! Default constructor
Pulsar::PlotPen::PlotPen ()
{
  line_width = 1;
  line_style = 1;
  colour_index = 1;
}

//! Set the plot line attributes
void Pulsar::PlotPen::setup ()
{
  cpgslw (line_width);
  cpgsls (line_style);
  cpgsci (colour_index);
}

