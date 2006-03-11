//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiPhase.h,v $
   $Revision: 1.7 $
   $Date: 2006/03/11 22:33:50 $
   $Author: straten $ */

#ifndef __Pulsar_MultiPhase_h
#define __Pulsar_MultiPhase_h

#include "Pulsar/PhasePlot.h"
#include "Pulsar/MultiFrame.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiPhase : public PhasePlot {

  public:

    //! Default constructor
    MultiPhase ();

    //! Get the text interface to the frame attributes
    TextInterface::Class* get_frame_interface ();

    //! Plot in the current viewport
    void plot (const Archive*);

    //! Manage a plotter
    void manage (const std::string& name, PhasePlot* plot);

    //! Set the viewport of the named plotter
    void set_viewport (const std::string& name, 
		       float x0, float x1, float y0, float y1);

  protected:

    //! The plot frames
    MultiFrame frames;

    //! The plotters
    std::map< std::string, Reference::To<PhasePlot> > plotters;

    void draw (const Archive*) {}

  };

}

#endif
