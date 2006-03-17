//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Attic/PlotFrameSize.h,v $
   $Revision: 1.7 $
   $Date: 2006/03/17 13:34:49 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrameSize_h
#define __Pulsar_PlotFrameSize_h

#include "Pulsar/PlotFrame.h"

#include <utility>

namespace Pulsar {

  //! PlotFrame with size
  class PlotFrameSize : public PlotFrame {

  public:

    //! Default constructor
    PlotFrameSize ();

    //! Partial copy constructor
    PlotFrameSize (const PlotFrame*);

    //! Text interface to the PlotFrameSize class
    class Interface : public TextInterface::To<PlotFrameSize> {
    public:
      Interface (PlotFrameSize* = 0);
    };

    //! Focus the frame
    void focus (const Archive*);

    //! Set the normalized coordinate of the viewport x axis
    void set_x_range (const std::pair<float,float>& range) { x_range=range; }
    std::pair<float,float> get_x_range () const { return x_range; }
 
    //! Set the normalized coordinate of the viewport y axis
    void set_y_range (const std::pair<float,float>& range) { y_range=range; }
    std::pair<float,float> get_y_range () const { return y_range; }

  protected:

    std::pair<float,float> x_range;
    std::pair<float,float> y_range;
    
  };

}

#endif
