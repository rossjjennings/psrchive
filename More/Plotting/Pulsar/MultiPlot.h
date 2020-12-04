//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/MultiPlot.h

#ifndef __Pulsar_MultiPlot_h
#define __Pulsar_MultiPlot_h

#include "Pulsar/Plot.h"
#include "Pulsar/MultiFrame.h"

namespace Pulsar {

  class FramedPlot;

  //! Plots multiple viewports
  class MultiPlot : public Plot {

  public:

    //! Default constructor
    MultiPlot ();

    //! Factory returns appropriate derived class
    static MultiPlot* factory (Plot*);

    //! Get the text interface to the frame attributes
    TextInterface::Parser* get_frame_interface ();

    //! Get the plot attributes
    PlotAttributes* get_attributes ();

    //! Plot in the current viewport
    void plot (const Archive*);

    //! Auto-manage a plot
    /*! Gives plot a default name and automatically scales all viewports.
     Throws an exception if Plot is not a Framed Plot. */
    void manage (Plot*);

    //! Manage a plot
    void manage (const std::string& name, FramedPlot*);

    //! Stop managing a plot
    void unmanage (FramedPlot*);

    //! Derived classes may wish to prepare before plotting
    virtual void prepare (const Archive*);

    //! Derived classes may wish to prepare before each plot is used
    virtual void prepare (FramedPlot*) { }

    //! Get the frame
    virtual MultiFrame* get_frame () { return &frames; }

    //! Get the frame
    virtual const MultiFrame* get_frame () const { return &frames; }

  protected:

    //! The plot frames
    MultiFrame frames;

    //! The plots
    std::map< std::string, Reference::To<FramedPlot> > plots;

    void set_viewport (PlotFrame* frame, 
		       std::pair<float,float>& sub_xvp,
		       std::pair<float,float>& sub_yvp);

  };

}

#endif
