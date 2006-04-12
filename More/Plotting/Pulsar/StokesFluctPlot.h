//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesFluctPlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/04/12 19:25:39 $
   $Author: straten $ */

#ifndef __Pulsar_StokesFluctPlot_h
#define __Pulsar_StokesFluctPlot_h

#include "Pulsar/FluctPlot.h"

namespace Pulsar {

  class Profile;

  //! Plots fluctuation power spectra
  class StokesFluctPlot : public FluctPlot {

  public:

    //! Default constructor
    StokesFluctPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Text interface to the StokesFluctPlot class
    class Interface : public TextInterface::To<StokesFluctPlot> {
    public:
      Interface (StokesFluctPlot* = 0);
    };

    //! Return the fluctuation power spectra to be plotted
    void get_profiles (const Archive*);

    //! Set the values to be plotted (one or more of I,Q,U,V,L,p)
    void set_plot_values (const std::string& plot) { plot_values = plot; }
    std::string get_plot_values () const { return plot_values; }

  protected:

    std::string plot_values;

  };

}

#endif
