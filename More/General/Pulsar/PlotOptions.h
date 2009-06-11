//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/PlotOptions.h,v $
   $Revision: 1.3 $
   $Date: 2009/06/11 09:01:10 $
   $Author: straten $ */

#ifndef __Pulsar_PlotOptions_h
#define __Pulsar_PlotOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Plot command line options
  class PlotOptions : public Application::Options
  {
  public:

    //! Default constructor
    PlotOptions ();

    //! Additional usage information implemented by derived classes
    std::string get_usage ();

    //! Additional getopt options
    std::string get_options ();

    //! Parse a command line option, return true if understood
    bool parse (char code, const std::string& arg);

    //! Open the plot device and configure it
    void setup ();

    //! Close the plot device
    void finalize ();

  private:

    std::string plot_device;

    // number of panel into which plot surface will be divided
    unsigned x_npanel, y_npanel;

    // width of plot surface in cm
    float surface_width;

    // aspect ratio (height/width)
    float aspect_ratio;

    // plot dimensions in pixels
    unsigned width_pixels, height_pixels;

  };

}

#endif
