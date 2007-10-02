//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FramedPlot.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 05:08:15 $
   $Author: straten $ */

#ifndef __Pulsar_FramedPlot_h
#define __Pulsar_FramedPlot_h

#include "Pulsar/Plot.h"
#include "Pulsar/PlotFrame.h"

namespace Pulsar {

  //! A Plot with a PlotFrame
  class FramedPlot : public Plot {

  public:

    //! Default constructor
    FramedPlot ();

    //! Get the text interface to the frame attributes
    virtual TextInterface::Parser* get_frame_interface ();

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
