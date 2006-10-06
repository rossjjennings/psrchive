//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiPlot.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_MultiPlot_h
#define __Pulsar_MultiPlot_h

#include "Pulsar/Plot.h"
#include "Pulsar/MultiFrame.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiPlot : public Plot {

  public:

    //! Default constructor
    MultiPlot ();

    //! Get the text interface to the frame attributes
    TextInterface::Class* get_frame_interface ();

    //! Plot in the current viewport
    void plot (const Archive*);

    //! Manage a plot
    void manage (const std::string& name, Plot* plot);

    //! Derived classes may wish to prepare before plotting
    virtual void prepare (const Archive*) {}

    //! Derived classes may wish to prepare before each plot is used
    virtual void prepare (Plot*) {}

    //! Set the viewport of the named plot
    void set_viewport (const std::string& name, 
		       float x0, float x1, float y0, float y1);

  protected:

    //! The plot frames
    MultiFrame frames;

    //! The plots
    std::map< std::string, Reference::To<Plot> > plots;

  };

}

#endif
