//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/More/Plotting/Pulsar/HasPen.h

#ifndef __Pulsar_HasPen_h
#define __Pulsar_HasPen_h

#include "PlotPen.h"

namespace Pulsar {

  //! Interface to objects that have a PlotPen
  class HasPen
  {
    PlotPen _pen;

  public:

    //! Provide access to the Pen
    PlotPen* get_pen () { return &_pen; }
  };

}

#endif
