/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotPen.h"

#include <cpgplot.h>

#include <iostream>
using namespace std;

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
  /*
  cerr << "Pulsar::PlotPen::setup"
    " width=" << line_width <<
    " style=" << line_style <<
    " colour=" << colour_index << endl;
  */

  cpgslw (line_width);
  cpgsls (line_style);
  cpgsci (colour_index);
}


Pulsar::PlotPen::Interface::Interface (PlotPen* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotPen::get_colour_index,
       &PlotPen::set_colour_index,
       "ci", "Colour index" );

  add( &PlotPen::get_line_style,
       &PlotPen::set_line_style,
       "ls", "Line style" );

  add( &PlotPen::get_line_width,
       &PlotPen::set_line_width,
       "lw", "Line width" );
}
