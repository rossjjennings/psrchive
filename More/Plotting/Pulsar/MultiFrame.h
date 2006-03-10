//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrame.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/10 21:32:18 $
   $Author: straten $ */

#ifndef __Pulsar_MultiFrame_h
#define __Pulsar_MultiFrame_h

#include "Pulsar/PlotFrameSize.h"
#include <map>

namespace Pulsar {

  //! Manages multiple plot frames
  class MultiFrame : public Reference::Able {

  public:

    //! Construct a new PlotFrameSize from the given PlotFrame and map it
    PlotFrameSize* manage (const std::string& name, PlotFrame*);

    //! Return a previously mapped plot frame
    PlotFrameSize* get_frame (const std::string& name);

    //! Return the shared x-axis
    PlotAxis* get_shared_x_axis() { return x_axis; }
    bool has_shared_x_axis () { return x_axis; }
    void set_shared_x_axis (PlotAxis* x) { x_axis = x; }

    //! Return the shared y-axis
    PlotAxis* get_shared_y_axis() { return y_axis; }
    bool has_shared_y_axis () { return y_axis; }
    void set_shared_y_axis (PlotAxis* y) { y_axis = y; }

  protected:

    std::map< std::string, Reference::To<PlotFrameSize> > frames;

    Reference::To<PlotAxis> x_axis;
    Reference::To<PlotAxis> y_axis;

  };

}

#endif
