//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrame.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/10 16:32:52 $
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

  protected:

    std::map< std::string, Reference::To<PlotFrameSize> > frames;
 
  };

}

#endif
