//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/StokesCovariancePlot.h

#ifndef __Pulsar_StokesCovariancePlot_h
#define __Pulsar_StokesCovariancePlot_h

#include "Pulsar/FluxPlot.h"

namespace Pulsar {

  //! Plots a polarization pulse profile
  class StokesCovariancePlot : public FluxPlot {

  public:

    //! Default constructor
    StokesCovariancePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the StokesCovariancePlot class
    class Interface : public TextInterface::To<StokesCovariancePlot> {
    public:
      Interface (StokesCovariancePlot* = 0);
    };

    //! Load the profiles
    void get_profiles (const Archive* data);

    enum What { Variances,
		EigenValues,
		NaturalCovariances,
		NonOrthogonality,
		Theta,
		RegressionCoefficients,
		ModulationIndex,
		DegreeOfPolarization};

    void set_what (What w) { what = w; }
    What get_what () const { return what; }

    //! Set the colour for each value plotted
    void set_plot_colours (const std::string& plot) { plot_colours = plot; }
    std::string get_plot_colours () const { return plot_colours; }

    //! Set the line style for each value plotted
    void set_plot_lines (const std::string& plot) { plot_lines = plot; }
    std::string get_plot_lines () const { return plot_lines; }

    //! Set the line width for each value plotted
    void set_plot_widths (const std::string& plot) { plot_widths = plot; }
    std::string get_plot_widths () const { return plot_widths; }

  private:

    What what;

    std::string plot_colours;
    std::string plot_lines;
    std::string plot_widths;

  };

  std::ostream& operator << (std::ostream& os, StokesCovariancePlot::What);

  std::istream& operator >> (std::istream& is, StokesCovariancePlot::What&);
}

#endif
