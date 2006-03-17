//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesPlot.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/17 13:34:50 $
   $Author: straten $ */

#ifndef __Pulsar_StokesPlot_h
#define __Pulsar_StokesPlot_h

#include "Pulsar/FluxPlot.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots a single pulse profile
  class StokesPlot : public FluxPlot {

  public:

    //! Default constructor
    StokesPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Text interface to the StokesPlot class
    class Interface : public TextInterface::To<StokesPlot> {
    public:
      Interface (StokesPlot* = 0);
    };

    //! Load the profiles
    void get_profiles (const Archive* data);

    //! Set the values to be plotted (one or more of I,Q,U,V,L,p)
    void set_plot_values (const std::string& plot) { plot_values = plot; }
    std::string get_plot_values () const { return plot_values; }

    //! Set the colour for each value plotted
    void set_plot_colours (const std::string& plot) { plot_colours = plot; }
    std::string get_plot_colours () const { return plot_colours; }

    //! Set the line style for each value plotted
    void set_plot_lines (const std::string& plot) { plot_lines = plot; }
    std::string get_plot_lines () const { return plot_lines; }

  protected:

    std::string plot_values;
    std::string plot_colours;
    std::string plot_lines;

  };

}

#endif
