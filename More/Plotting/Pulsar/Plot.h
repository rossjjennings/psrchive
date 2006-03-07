//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/07 23:13:23 $
   $Author: straten $ */

#ifndef __Pulsar_Graph_h
#define __Pulsar_Graph_h

#include "Pulsar/PlotFrame.h"

namespace TextInterface {
  class Class;
}

namespace Pulsar {

  class Archive;

  class Graph : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Plot in the current viewport
    virtual void plot (const Archive*) = 0;

    //! Get the text interface to the attributes
    virtual TextInterface::Class* get_text_interface () = 0;

    //! Get the frame
    PlotFrame* get_frame () { return &frame; }

  protected:

    //! The plot frame
    PlotFrame frame;

  };

}

#endif
