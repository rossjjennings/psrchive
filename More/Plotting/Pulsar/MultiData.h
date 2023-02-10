//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/MultiData.h

#ifndef __Pulsar_MultiData_h
#define __Pulsar_MultiData_h

#include "Pulsar/Plot.h"

namespace Pulsar {

  //! Combines multiple data sets in a single plot
  class MultiData : public Plot
  {

  public:

    //! Factory returns appropriate derived class
    static Plot* factory (Plot*);

    //! Process the Archive as needed before calling plot
    void preprocess (Archive*);

    //! Provide access to the plot attributes
    PlotAttributes* get_attributes ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    //! Get the text interface to the frame attributes
    TextInterface::Parser* get_frame_interface ();

    //! Process a configuration command and store it for later use
    void configure (const std::string& option);

    //! Set the pre-processor
    void set_preprocessor (Processor*);

    //! Get the pre-processor
    Processor* get_preprocessor ();

    //! Return true if pre-processor has been set
    bool has_preprocessor () const;

    //! Get the managed plot
    Plot* get_Plot () { return managed_plot; }

  protected:

    //! The plot that is managed by this instance
    Reference::To<Plot> managed_plot;

    //! The options passed to configure
    std::vector< std::string > options;
  };

}

#endif
