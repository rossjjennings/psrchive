//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhasePlot.h,v $
   $Revision: 1.13 $
   $Date: 2006/03/14 16:08:15 $
   $Author: straten $ */

#ifndef __Pulsar_PhasePlot_h
#define __Pulsar_PhasePlot_h

#include "Pulsar/Plot.h"
#include "Pulsar/PhaseScale.h"

#include <vector>

namespace Pulsar {

  //! Plots with pulse phase along the x-axis
  /*! Children of the PhasePlot always:
    <UL>
    <LI> plot in the currently open viewport
    <LI> use the currently set line width
    <LI> use the currently set character height
    </UL>
    That is, children of the PhasePlot class do not call
    cpgsvp, cpgslw, or cpgsch.  Children of the PhasePlot class may:
    <UL>
    <LI> change the colour used within the plot frame
    <LI> change the line style within the plot frame
    </UL>
  */
  class PhasePlot : public Plot {

  public:

    //! Default constructor
    PhasePlot ();

    //! Destructor
    virtual ~PhasePlot ();

    //! Plot in the current viewport
    virtual void plot (const Archive*);

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Get the default label for the x axis
    virtual std::string get_xlabel (const Archive*);

    //! Get the default label for the y axis
    virtual std::string get_ylabel (const Archive*);

    //! Derived classes must prepare to draw
    /*! Derived classes must at least call set_yrange */
    virtual void prepare (const Archive*) = 0;

    //! Derived classes must draw in the current viewport
    virtual void draw (const Archive*) = 0;

    //! Get the scale
    PhaseScale* get_scale ();

    //! Get the min and max phase bins corresponding to x_axis->get_range
    void get_range_bin (const Archive*, unsigned& min, unsigned& max);

  protected:

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

}

#endif
