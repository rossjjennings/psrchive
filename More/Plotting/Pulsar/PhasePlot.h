//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhasePlot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/05 21:07:34 $
   $Author: straten $ */

#ifndef __Pulsar_ProfilePlotter_h
#define __Pulsar_ProfilePlotter_h

#include "Reference.h"

#include <vector>

namespace TextInterface {
  class Class;
}

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

    //! Automatically zoom in on the on-pulse region
    virtual void auto_zoom_phase (const Archive* data, float buffer);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned isubint);
    unsigned get_subint () const {return isubint;}
    
    //! Set the polarization to plot (where applicable)
    void set_pol (unsigned ipol);
    unsigned get_pol () const {return ipol;}
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned ichan);
    unsigned get_chan () const {return ichan;}

    //! Set the minimum phase in plot
    void set_min_phase (float min_phase);
    float get_min_phase () const { return min_phase; }

    //! Set the maximum phase in plot
    void set_max_phase (float max_phase);
    float get_max_phase () const { return max_phase; }

    //! Set the fractional height of maximum 
    void set_max_fraction (float max) { max_fraction = max; }
    float get_max_fraction () const { return max_fraction; }

    //! Set the fractional height of minimum
    void set_min_fraction (float min) { min_fraction = min; }
    float get_min_fraction () const { return min_fraction; }

    //! Set the label of the x-axis
    void set_abscissa (Abscissa);

    //! Plot an error box on the baseline
    void set_plot_error_box (bool flag = true) { plot_error = flag; }
    bool get_plot_error_box () const { return plot_error; }

    //! Print the axes around the plot area
    void set_plot_axes (bool _axes = true) { axes = _axes; }
    bool get_plot_axes () const { return axes; }
  
    //! Enumerate the x axis
    void set_x_enumerate (bool val = true) { x_enumerate = val; }
    bool get_x_enumerate () const { return x_enumerate; }

    //! Enumerate the y-axis
    void set_y_enumerate (bool val = true) { y_enumerate = val; }
    bool get_y_enumerate () const { return y_enumerate; }

    //! Label the x-axis
    void set_x_label (bool val = true) { x_label = val; }
    bool get_x_label () const { return x_label; }

    //! Label the y-axis
    void set_y_label (bool val = true) { y_label = val; }
    bool get_y_label () const { return y_label; }

    //! Get the text interface to the attributes
    TextInterface::Class* get_text_interface ();

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

    Reference::To<TextInterface::Class> text_interface;

  };

}

#endif
