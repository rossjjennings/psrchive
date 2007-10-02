//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.15 $
   $Date: 2007/10/02 05:08:15 $
   $Author: straten $ */

#ifndef __Pulsar_Plot_h
#define __Pulsar_Plot_h

#include "TextInterface.h"

namespace Pulsar {

  class Archive;

  //! Base class of all plotters
  class Plot : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Plot in the current viewport
    virtual void plot (const Archive*) = 0;

    //! Get the text interface to the configuration attributes
    virtual TextInterface::Parser* get_interface () = 0;

    //! Get the text interface to the frame attributes
    virtual TextInterface::Parser* get_frame_interface () = 0;

    //! Process a configuration command
    virtual void configure (const std::string&);

  private:

    // used by the configure method
    Reference::To<TextInterface::Parser> tui;
    Reference::To<TextInterface::Parser> fui;

  };

}

#endif
