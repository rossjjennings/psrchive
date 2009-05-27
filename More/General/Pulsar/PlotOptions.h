//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/PlotOptions.h,v $
   $Revision: 1.2 $
   $Date: 2009/05/27 23:52:26 $
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
    virtual std::string get_usage ();

    //! Additional getopt options
    virtual std::string get_options ();

    //! Parse a command line option, return true if understood
    virtual bool parse (char code, const std::string& arg);

    //! Open the plot device and configure it
    virtual void setup ();

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
