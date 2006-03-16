//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrame.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/16 17:07:17 $
   $Author: straten $ */

#ifndef __Pulsar_MultiFrame_h
#define __Pulsar_MultiFrame_h

#include "Pulsar/PlotFrameSize.h"
#include <map>

namespace Pulsar {

  //! Manages multiple plot frames
  class MultiFrame : public Reference::Able {

  public:

    //! Text interface to the MultiFrame class
    class Interface : public TextInterface::To<MultiFrame> {
    public:
      Interface (MultiFrame* = 0);
    };

    //! Construct a new PlotFrameSize from the given PlotFrame and map it
    PlotFrameSize* manage (const std::string& name, PlotFrame*);

    //! Return a previously mapped plot frame
    PlotFrameSize* get_frame (const std::string& name);

    //! Return the shared x-scale
    PlotScale* get_shared_x_scale() { return x_scale; }
    bool has_shared_x_scale () { return x_scale; }
    void set_shared_x_scale (PlotScale* x) { x_scale = x; }

    //! Return the shared y-scale
    PlotScale* get_shared_y_scale() { return y_scale; }
    bool has_shared_y_scale () { return y_scale; }
    void set_shared_y_scale (PlotScale* y) { y_scale = y; }

  protected:

    std::map< std::string, Reference::To<PlotFrameSize> > frames;

    Reference::To<PlotScale> x_scale;
    Reference::To<PlotScale> y_scale;

  };

}

#endif
