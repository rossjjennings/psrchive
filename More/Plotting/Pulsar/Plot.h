//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.13 $
   $Date: 2007/06/04 19:38:08 $
   $Author: straten $ */

#ifndef __Pulsar_Plot_h
#define __Pulsar_Plot_h

#include "Pulsar/PlotAttributes.h"
#include "Pulsar/PlotFrame.h"

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

    //! Process a configuration command
    virtual void configure (const std::string&);

    //! Get the attributes
    virtual PlotAttributes* get_attributes ();

    //! Get the attributes
    virtual const PlotAttributes* get_attributes () const;

    //! Set the attributes
    virtual void set_attributes (PlotAttributes*);

    //! Get the frame
    virtual PlotFrame* get_frame ();

    //! Get the frame
    virtual const PlotFrame* get_frame () const;

    //! Set the frame
    virtual void set_frame (PlotFrame*);

  protected:

    //! The plot attributes
    Reference::To<PlotAttributes> attributes;

    //! The plot frame
    Reference::To<PlotFrame> frame;

  private:

    // used by the configure method
    Reference::To<TextInterface::Class> tui;
    Reference::To<TextInterface::Class> fui;

  };

}

#endif
