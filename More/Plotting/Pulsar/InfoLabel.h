//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/InfoLabel.h,v $
   $Revision: 1.1 $
   $Date: 2006/10/03 16:45:01 $
   $Author: straten $ */

#ifndef __Pulsar_InfoLabel_h
#define __Pulsar_InfoLabel_h

#include "Pulsar/PlotLabel.h"

namespace Pulsar {

  //! Print a label with three rows above the plot frame
  /*! This class can be used to emulate the behaviour of the old
    Plotter::singleProfile method */

  class InfoLabel : public PlotLabel {

  public:

    //! Plot the label
    virtual void plot (const Archive*);

  };

}

#endif
