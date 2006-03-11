//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrame.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/11 22:14:46 $
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

    //! Return the shared x-zoom
    PlotZoom* get_shared_x_zoom() { return x_zoom; }
    bool has_shared_x_zoom () { return x_zoom; }
    void set_shared_x_zoom (PlotZoom* x) { x_zoom = x; }

    //! Return the shared y-zoom
    PlotZoom* get_shared_y_zoom() { return y_zoom; }
    bool has_shared_y_zoom () { return y_zoom; }
    void set_shared_y_zoom (PlotZoom* y) { y_zoom = y; }

  protected:

    std::map< std::string, Reference::To<PlotFrameSize> > frames;

    Reference::To<PlotZoom> x_zoom;
    Reference::To<PlotZoom> y_zoom;

  };

}

#endif
