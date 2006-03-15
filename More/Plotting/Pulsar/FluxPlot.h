//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FluxPlot.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/15 11:38:50 $
   $Author: straten $ */

#ifndef __Pulsar_FluxPlot_h
#define __Pulsar_FluxPlot_h

#include "Pulsar/PhasePlot.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class FluxPlot : public PhasePlot {

  public:

    //! Default constructor
    FluxPlot ();

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
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

    //! Set the polarization to plot
    void set_pol (unsigned _ipol) { ipol = _ipol; }
    unsigned get_pol () const { return ipol; }

    //! Plot an error box on the baseline
    void set_plot_error_box (bool flag = true) { plot_ebox = flag; }
    bool get_plot_error_box () const { return plot_ebox; }

    //! Plot an error box on the baseline
    void set_plot_histogram (bool flag = true) { plot_histogram = flag; }
    bool get_plot_histogram () const { return plot_histogram; }

    //! Derived classes must draw in the current viewport
    void draw (const Archive*);

    //! draw the profile in the current viewport and window
    void draw (const Profile*) const;

    //! Return the text interface
    TextInterface::Class* get_interface ();

  protected:

    std::vector< Reference::To<const Profile> > profiles;
    std::vector< int > plot_sci;
    std::vector< int > plot_sls;

    unsigned isubint;
    unsigned ichan;
    unsigned ipol;

    bool plot_ebox;
    bool plot_histogram;

  private:
    std::vector<float> phases;

  };

}

#endif
