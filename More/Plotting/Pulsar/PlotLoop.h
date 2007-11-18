//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotLoop.h,v $
   $Revision: 1.1 $
   $Date: 2007/11/18 11:56:44 $
   $Author: straten $ */

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

    //! Set the Plot to be executed
    void set_Plot (Plot*);

    //! Set the Archive to be plotted
    void set_Archive (const Archive*);

    //! Set the overlay flag
    void set_overlay (bool);

    //! Get the overlay flag
    bool get_overlay () const;

    //! Add an index over which to loop
    void add_index (TextIndex*);

    // execute the plot for each index in the stack
    void plot ();

  protected:

    Reference::To<Plot> the_plot;

    Reference::To<const Archive> archive;

    bool overlay;

    std::stack< Reference::To<TextIndex> > index_stack;

    //! Worker function called recursively down stack
    void plot ( std::stack< Reference::To<TextIndex> >& indeces );

  };

}

#endif
