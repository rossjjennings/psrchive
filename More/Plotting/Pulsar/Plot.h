//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/09 22:29:53 $
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

    //! Get the text interface to the configuration attributes
    virtual TextInterface::Class* get_interface () = 0;

    //! Get the text interface to the frame attributes
    virtual TextInterface::Class* get_frame_interface () = 0;

  };

}

#endif
