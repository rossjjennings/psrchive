//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhasePlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/03 23:23:36 $
   $Author: straten $ */

#ifndef __Pulsar_ProfilePlotter_h
#define __Pulsar_ProfilePlotter_h

#include "Reference.h"

#include <vector>

namespace Pulsar {

  class Archive;

  //! Plots with pulse phase along the x-axis
  /*! Children of the ProfilePlotter always:
    <UL>
    <LI> plot in the currently open viewport
    <LI> use the currently set line width
    <LI> use the currently set character height
    </UL>
    That is, children of the ProfilePlotter class do not call
    cpgsvp, cpgslw, or cpgsch.  Children of the ProfilePlotter class may:
    <UL>
    <LI> change the colour used within the plot frame
    <LI> change the line style within the plot frame
    </UL>
  */
  class ProfilePlotter : public Reference::Able {

  public:

    //! The units on the x-axis
    enum Abscissa { Turns, Degrees, Radians, Milliseconds };

    static bool verbose;

    //! Default constructor
    ProfilePlotter ();

    //! Destructor
    virtual ~ProfilePlotter ();

    //! Plot in the current viewport
    virtual void plot (const Archive*);

    //! Derived classes must compute the minimum and maximum values (y-axis)
    virtual void minmax (const Archive*, float& min, float& max) = 0;

    //! Derived classes must draw in the current viewport
    virtual void draw (const Archive*) = 0;

    //! Return the label for the y-axis
    virtual std::string get_flux_label (const Archive* data);

    //! Return the label for the x-axis
    virtual std::string get_phase_label (const Archive* data);

    //! Compute the error in phase
    virtual float get_phase_error (const Archive* data);

    //! Compute the error in flux
    virtual float get_flux_error (const Archive* data);

    //! Draw the error box
    virtual void plot_error_box (const Archive* data);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned isubint);
    
    //! Get the sub-integration that will be plotted (where applicable)
    unsigned get_subint () {return isubint;}
    
    //! Set the polarization to plot (where applicable)
    void set_pol (unsigned ipol);
    
    //! Get the polarization that will be plotted (where applicable)
    unsigned get_pol () {return ipol;}
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned ichan);
    
    //! Get the channel that will be plotted (where applicable)
    unsigned get_chan () {return ichan;}

    //! Zoom in on a pulse window.
    void set_zoom (float min_phase, float max_phase);
    
    //! Automatically zoom in on the on-pulse region
    void auto_zoom (const Archive* data, float buffer);

    //! Set the maximum fractional intensity in plot
    void set_max_fraction (float max);

    //! Set the minimum fractional intensity in plot
    void set_min_fraction (float min);

    //! Set the label of the x-axis
    void set_abscissa (Abscissa);

    //! Plot an error box on the baseline
    void set_plot_error_box (bool flag = true) { plot_error = flag; }
    bool get_plot_error_box () const { return plot_error; }

    //! Print the axes around the plot area
    void set_plot_axes (bool _axes = true) { axes = _axes; }
    bool get_plot_axes () const { return axes; }
        
  protected:

    unsigned isubint;
    unsigned ipol;
    unsigned ichan;

    //! Abscissa scale
    Abscissa abscissa;

    //! Minimum phase in plot
    float min_phase;

    //! Maximum phase in plot
    float max_phase;

    //! Fraction of maxiumum plotted
    float max_fraction;

    //! Fraction above minimum plotted
    float min_fraction;

    //! Fraction of max-min used as a border
    float border;

    //! Plot axes around the box
    bool axes;

    //! Enumerate the x-axis
    bool x_enumerate;
    bool x_label;

    //! Enumerate the y-axis
    bool y_enumerate;
    bool y_label;

    //! Include error box somewhere in plot
    bool plot_error;

    std::vector<float> phases;

  };

}

#endif
