//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/09 23:02:02 $
   $Author: straten $ */

#ifndef __Pulsar_Graph_h
#define __Pulsar_Graph_h

#include "Reference.h"

namespace TextInterface {
  class Class;
}

namespace Pulsar {

  class Archive;

  //! Pure virtual base class of all plotters
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
