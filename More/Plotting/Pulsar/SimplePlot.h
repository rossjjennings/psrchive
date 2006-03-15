//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/SimplePlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/15 11:38:40 $
   $Author: straten $ */

#ifndef __Pulsar_SimplePlot_h
#define __Pulsar_SimplePlot_h

#include "Pulsar/Plot.h"

namespace Pulsar {

  //! Plots a single plot
  class SimplePlot : public Plot {

  public:

    //! Plot in the current viewport
    virtual void plot (const Archive*);

    //! Get the default label for the x axis
    virtual std::string get_xlabel (const Archive*) = 0;

    //! Get the default label for the y axis
    virtual std::string get_ylabel (const Archive*) = 0;

    //! Derived classes must prepare to draw
    virtual void prepare (const Archive*) = 0;

    //! Derived classes must draw in the current viewport
    virtual void draw (const Archive*) = 0;

  };

}

#endif
