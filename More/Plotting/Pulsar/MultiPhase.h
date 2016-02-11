//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/MultiPhase.h

#ifndef __Pulsar_MultiPhase_h
#define __Pulsar_MultiPhase_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/HasPhaseScale.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiPhase : public MultiPlot, public HasPhaseScale
  {

  public:

    //! Default constructor
    MultiPhase ();

    // Text interface to the MultiPhase class
    class Interface : public TextInterface::To<MultiPhase> {
    public:
      Interface (MultiPhase* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    //! Get the scale
    PhaseScale* get_scale ();

  };

}

#endif
