//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesPlot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/09 20:35:57 $
   $Author: straten $ */

#ifndef __Pulsar_StokesPlotter_h
#define __Pulsar_StokesPlotter_h

#include "Pulsar/FluxPlotter.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots a single pulse profile
  class StokesPlotter : public FluxPlotter {

  public:

    //! Default constructor
    StokesPlotter ();

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
