//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/More/Plotting/Pulsar/PlotPen.h

#ifndef __Pulsar_PlotPen_h
#define __Pulsar_PlotPen_h

#include "TextInterface.h"

namespace Pulsar {

  //! Stores and sets plot line attributes
  class PlotPen : public Reference::Able
  {
    int colour_index;
    int line_style;
    int line_width;

  public:

    //! Default constructor
    PlotPen ();

    //! Set the plot line attributes
    void setup ();

    //! Set the colour for each value plotted
    void set_colour_index (const int val) { colour_index = val; }
    int get_colour_index () const { return colour_index; }

    //! Set the line style for each value plotted
    void set_line_style (const int val) { line_style = val; }
    int get_line_style () const { return line_style; }

    //! Set the line width for each value plotted
    void set_line_width (const int val) { line_width = val; }
    int get_line_width () const { return line_width; }

    // Text interface to the PlotPen class
    class Interface : public TextInterface::To<PlotPen> {
    public:
      Interface (PlotPen* = 0);
    };

  };

}

#endif
