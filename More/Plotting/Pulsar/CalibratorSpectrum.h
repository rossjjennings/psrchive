//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/CalibratorSpectrum.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/07 13:50:19 $
   $Author: straten $ */

#ifndef __Pulsar_CalibratorSpectrum_h
#define __Pulsar_CalibratorSpectrum_h

#include "Pulsar/FrequencyPlot.h"
#include "Pulsar/PlotIndex.h"
#include "EstimatePlotter.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class CalibratorSpectrum : public FrequencyPlot {

  public:

    //! Default constructor
    CalibratorSpectrum ();

    //! Return the text interface
    TextInterface::Class* get_interface ();

    // Text interface to the CalibratorSpectrum class
    class Interface : public TextInterface::To<CalibratorSpectrum> {
    public:
      Interface (CalibratorSpectrum* = 0);
    };

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const PlotIndex& _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }

    //! Plot the total and polarized intensities
    void set_plot_Ip (bool flag = true) { plot_Ip = flag; }
    bool get_plot_Ip () const { return plot_Ip; }

    //! Plot the total and polarized intensities
    void set_plot_total (bool flag = true) { plot_total = flag; }
    bool get_plot_total () const { return plot_total; }

    //! Plot the total and polarized intensities
    void set_plot_low (bool flag = true) { plot_low = flag; }
    bool get_plot_low () const { return plot_low; }

    //! Prepare to plot
    void prepare (const Archive*);

    //! Draw the spectra
    void draw (const Archive*);

  protected:

    PlotIndex isubint;
    bool plot_total;
    bool plot_low;
    bool plot_Ip;

    EstimatePlotter plotter;

  };

}

#endif
