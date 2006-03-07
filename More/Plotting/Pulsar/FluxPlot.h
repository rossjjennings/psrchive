//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FluxPlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/07 23:13:23 $
   $Author: straten $ */

#ifndef __Pulsar_FluxPlotter_h
#define __Pulsar_FluxPlotter_h

#include "Pulsar/ProfilePlotter.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class FluxPlotter : public ProfilePlotter {

  public:

    //! Default constructor
    FluxPlotter ();

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Derived classes must provide the profiles
    virtual void get_profiles (const Archive*) = 0;

    //! Compute the error in phase
    virtual float get_phase_error (const Archive* data);

    //! Compute the error in flux
    virtual float get_flux_error (const Profile* data);
    std::string get_flux_label (const Archive* data);

    //! Draw the error box
    virtual void plot_error_box (const Archive* data);

    //! Automatically zoom in on the on-pulse region
    virtual void auto_zoom_phase (const Profile* data, float buffer);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

    //! Plot an error box on the baseline
    void set_plot_error_box (bool flag = true) { plot_ebox = flag; }
    bool get_plot_error_box () const { return plot_ebox; }

    //! Plot an error box on the baseline
    void set_plot_histogram (bool flag = true) { plot_histogram = flag; }
    bool get_plot_histogram () const { return plot_histogram; }

    //! Derived classes must draw in the current viewport
    void draw (const Archive*);

    //! draw the profile in the current viewport and window
    void draw (const Profile* profile) const;

  protected:

    std::vector< Reference::To<const Profile> > profiles;

    unsigned isubint;
    unsigned ichan;
    bool plot_ebox;
    bool plot_histogram;

  };

}

#endif
