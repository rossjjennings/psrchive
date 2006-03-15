//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/15 16:35:04 $
   $Author: straten $ */

#ifndef __Pulsar_Plot_h
#define __Pulsar_Plot_h

#include "Pulsar/PlotFrame.h"

namespace TextInterface {
  class Class;
}

namespace Pulsar {

  class Archive;

  //! Base class of all plotters
  class Plot : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    Plot ();

    //! Plot in the current viewport
    virtual void plot (const Archive*) = 0;

    //! Get the text interface to the configuration attributes
    virtual TextInterface::Class* get_interface () = 0;

    //! Get the text interface to the frame attributes
    virtual TextInterface::Class* get_frame_interface ();

    //! Get the frame
    virtual PlotFrame* get_frame ();

    //! Get the frame
    virtual const PlotFrame* get_frame () const;

    //! Set the frame
    virtual void set_frame (PlotFrame*);

  protected:

    //! The plot frame
    Reference::To<PlotFrame> frame;

  };

}

#endif
