//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/PlotLoop.h

#ifndef __Pulsar_PlotLoop_h
#define __Pulsar_PlotLoop_h

#include "TextIndex.h"
#include <stack>

namespace Pulsar {

  class Archive;
  class Plot;

  //! Base class of all plotters
  class PlotLoop : public Reference::Able {

  public:

    //! Default constructor
    PlotLoop ();

    //! Add a Plot to be executed
    void add_Plot (Plot*);

    //! Configure the plots
    void configure (const std::vector<std::string>& options);

    //! Perform any cleanup
    void finalize ();

    //! Set the Archive to be plotted
    void set_Archive (Archive*);

    //! Overlay the data from multiple objects in a single plot
    void set_overlay (bool);
    bool get_overlay () const;

    //! Automatically stack multiple plots
    void set_stack (bool);
    bool get_stack () const;

    //! Set the colour indeces through which to loop
    void set_colour_indeces (const std::vector<unsigned>& ci);

    //! Set the preprocess flag
    void set_preprocess (bool);

    //! Get the preprocess flag
    bool get_preprocess () const;

    //! Add an index over which to loop
    void add_index (TextIndex*);

    // execute the plot for each index in the stack
    void plot ();

    // set the colour for the plot
    void set_colour_index (Plot*, unsigned colour_index);

  protected:

    std::vector< Reference::To<Plot> > plots;

    std::vector< Reference::To<Archive> > archives;

    bool preprocess;
    bool overlay;
    bool stack;

    std::stack< Reference::To<TextIndex> > index_stack;

    std::vector<unsigned> colour_indeces;
    unsigned current_colour_index;

    //! Worker function called recursively down stack
    void plot ( std::stack< Reference::To<TextIndex> >& indeces );

  };

}

#endif
