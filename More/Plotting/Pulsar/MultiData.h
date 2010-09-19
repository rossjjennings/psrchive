//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiData.h,v $
   $Revision: 1.1 $
   $Date: 2010/09/19 01:23:02 $
   $Author: straten $ */

#ifndef __Pulsar_MultiData_h
#define __Pulsar_MultiData_h

#include "Pulsar/Plot.h"

namespace Pulsar {

  //! Combines multiple data sets in a single plot
  class MultiData : public Plot
  {

  public:

    //! Factory returns appropriate derived class
    static MultiData* factory (Plot*);

    //! Process the Archive as needed before calling plot
    void preprocess (Archive*);

    //! Provide access to the plot attributes
    PlotAttributes* get_attributes ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    //! Get the text interface to the frame attributes
    TextInterface::Parser* get_frame_interface ();

  protected:

    //! The plot that is managed by this instance
    Reference::To<Plot> managed_plot;
  };

}

#endif
