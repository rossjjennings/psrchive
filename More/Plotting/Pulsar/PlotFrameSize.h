//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Attic/PlotFrameSize.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/10 05:06:59 $
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

    //! Set the normalized coordinate of the bottom left corner
    void set_bottom_left (const std::pair<float,float>& c) { bottom_left=c; }
    std::pair<float,float> get_bottom_left () const { return bottom_left; }
 
    //! Set the normalized coordinate of the top right corner
    void set_top_right (const std::pair<float,float>& c) { top_right=c; }
    std::pair<float,float> get_top_right () const { return top_right; }

  protected:

    std::pair<float,float> bottom_left;
    std::pair<float,float> top_right;
    
  };

}

#endif
