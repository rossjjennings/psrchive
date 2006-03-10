//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhasePlot.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/10 20:37:58 $
   $Author: straten $ */

#ifndef __Pulsar_ProfilePlotter_h
#define __Pulsar_ProfilePlotter_h

#include "Pulsar/Graph.h"
#include "Pulsar/PlotFrame.h"

#include <vector>

namespace Pulsar {

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
  class ProfilePlotter : public Graph {

  public:

    //! The units on the phase axis
    enum Scale { Turns, Degrees, Radians, Milliseconds };

    //! Default constructor
    ProfilePlotter ();

    //! Destructor
    virtual ~ProfilePlotter ();

    //! Plot in the current viewport
    virtual void plot (const Archive*);

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Get the text interface to the frame attributes
    TextInterface::Class* get_frame_interface ();

    //! Get the default label for the x axis
    virtual std::string get_xlabel (const Archive*);

    //! Get the default label for the y axis
    virtual std::string get_ylabel (const Archive*);

    //! Derived classes must prepare to draw
    /*! Derived classes must at least call set_yrange */
    virtual void prepare (const Archive*) = 0;

    //! Derived classes must draw in the current viewport
    virtual void draw (const Archive*) = 0;

    //! Set the scale on the phase axis
    void set_scale (Scale s) { scale = s; }
    //! Get the scale on the phase axis
    Scale get_scale () const { return scale; }

    //! Set the world-normalized coordinates of the origin on the phase axis
    void set_origin_norm (float f) { origin_norm = f; }
    //! Get the world-normalized coordinates of the origin on the phase axis
    float get_origin_norm () const { return origin_norm; }

    //! Get the frame
    PlotFrame* get_frame () { return frame; }
    //! Set the frame
    void set_frame (PlotFrame* f) { frame = f; }

  protected:

    //! The plot frame
    Reference::To<PlotFrame> frame;

    //! Scale on the phase axis
    Scale scale;

    //! Origin on the phase axis
    float origin_norm;

    //! Phases can be used
    std::vector<float> phases;

    //! Derived classes must set the range in y values during prepare
    void set_yrange (float min, float max);

  private:

    //! Set true when yrange is called
    bool yrange_set;

    //! The minimum value of y
    float y_min;

    //! The maximum value of y
    float y_max;

  };

  std::ostream& operator << (std::ostream& os, ProfilePlotter::Scale);
  std::istream& operator >> (std::istream& is, ProfilePlotter::Scale&);

}

#endif
