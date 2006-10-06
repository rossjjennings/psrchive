//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FluxPlot.h,v $
   $Revision: 1.19 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_FluxPlot_h
#define __Pulsar_FluxPlot_h

#include "Pulsar/PhasePlot.h"
#include "Pulsar/ProfileVectorPlotter.h"
#include "Pulsar/PlotIndex.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class FluxPlot : public PhasePlot {

  public:

    //! Default constructor
    FluxPlot ();

    //! Destructor
    ~FluxPlot ();

    //! Return the text interface
    TextInterface::Class* get_interface ();

    //! Text interface to the FluxPlot class
    class Interface : public TextInterface::To<FluxPlot> {
    public:
      Interface (FluxPlot* = 0);
    };

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the profiles
    virtual void get_profiles (const Archive*) = 0;

    //! Compute the error in phase
    virtual float get_phase_error (const Archive* data);

    //! Compute the error in flux
    virtual float get_flux_error (const Profile* data);

    //! Draw the error box
    virtual void plot_error_box (const Archive* data);

    //! Automatically scale in on the on-pulse region
    virtual void auto_scale_phase (const Profile* data, float buffer);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (PlotIndex _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (PlotIndex _ichan) { ichan = _ichan; }
    PlotIndex get_chan () const { return ichan; }

    //! Set the polarization to plot
    void set_pol (PlotIndex _ipol) { ipol = _ipol; }
    PlotIndex get_pol () const { return ipol; }

    //! Automatically zoom in on the on-pulse region
    void set_auto_zoom (float zoom) { auto_zoom = zoom; }
    float get_auto_zoom () const { return auto_zoom; }

    //! Plot an error box on the baseline
    void set_plot_error_box (bool flag = true) { plot_ebox = flag; }
    bool get_plot_error_box () const { return plot_ebox; }

    //! Plot an error box on the baseline
    void set_plot_histogram (bool f = true) { plotter.plot_histogram = f; }
    bool get_plot_histogram () const { return plotter.plot_histogram; }

    //! Set the original number of frequency channels
    /*! This attribute is used in the get_phase_error method to determine
      the dispersion smearing across each channel */
    void set_original_nchan (unsigned nchan) { original_nchan = nchan; }
    unsigned get_original_nchan () const { return original_nchan; }

    //! Draw the profiles in the currently open window
    void draw (const Archive*);

    //! Draw a profile in the currently open window
    void plot_profile (const Profile*);

    //! Provide limited access to the plotter
    const ProfileVectorPlotter* get_plotter () const { return &plotter; }

  protected:

    ProfileVectorPlotter plotter;

    PlotIndex isubint;
    PlotIndex ichan;
    PlotIndex ipol;

    bool plot_ebox;
    float auto_zoom;

    unsigned original_nchan;
  };

}

#endif
