//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiPhase.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/10 21:32:18 $
   $Author: straten $ */

#ifndef __Pulsar_MultiProfile_h
#define __Pulsar_MultiProfile_h

#include "Pulsar/ProfilePlotter.h"
#include "Pulsar/MultiFrame.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiProfile : public ProfilePlotter {

  public:

    //! Default constructor
    MultiProfile ();

    //! Get the text interface to the frame attributes
    TextInterface::Class* get_frame_interface ();

    //! Plot in the current viewport
    void plot (const Archive*);

    //! Manage a plotter
    void manage (const std::string& name, ProfilePlotter* plot);

    //! Set the viewport of the named plotter
    void set_viewport (const std::string& name, 
		       float x0, float x1, float y0, float y1);

  protected:

    //! The plot frames
    MultiFrame frames;

    //! The plotters
    std::map< std::string, Reference::To<ProfilePlotter> > plotters;

    void draw (const Archive*) {}

  };

}

#endif
