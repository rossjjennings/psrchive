//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/MultiFrequency.h

#ifndef __Pulsar_MultiFrequency_h
#define __Pulsar_MultiFrequency_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/FrequencyScale.h"

namespace Pulsar {

  //! Plots multiple viewports with radio frequency along the shared x-axis
  class MultiFrequency : public MultiPlot {

  public:

    //! Default constructor
    MultiFrequency ();

    // Text interface to the MultiFrequency class
    class Interface : public TextInterface::To<MultiFrequency> {
    public:
      Interface (MultiFrequency* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    //! Get the scale
    FrequencyScale* get_scale ();

  };

}

#endif
