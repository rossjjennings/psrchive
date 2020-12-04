//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/MultiDataPlot.h

#ifndef __Pulsar_MultiDataPlot_h
#define __Pulsar_MultiDataPlot_h

#include "Pulsar/MultiData.h"

#include "Pulsar/SimplePlot.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/MultiFrame.h"
#include "Pulsar/Archive.h"

#include <iostream>
using namespace std;

namespace Pulsar {

  //! Combines multiple data sets in a single plot
  template<class Type> class MultiDataPlot : public MultiData 
  {

  public:

    //! Construct with plot to be managed
    MultiDataPlot (Type* simple = 0);

    //! Manage a plot
    void manage (Type*);

    //! prepare the Simple Plot and push it onto the plots vector
    void plot (const Archive*);

    //! draw the accumulated plots
    void finalize ();

  protected:

    Reference::To<Type> the_plot;

    std::vector< Reference::To<Type> > plots;
    std::vector< Reference::To<const Archive> > data_sets;

  };

}

//! Construct with plot to be managed
template<class Type>
Pulsar::MultiDataPlot<Type>::MultiDataPlot (Type* simple)
{
  manage (simple);
}

//! Manage a plot
template<class Type>
void Pulsar::MultiDataPlot<Type>::manage (Type* simple)
{
  managed_plot = simple;
  the_plot = simple;
}

//! prepare the Simple Plot and push it onto the plots vector
template<class Type>
void Pulsar::MultiDataPlot<Type>::plot (const Archive* data)
{
  the_plot->get_frame()->init (data);

  if (verbose)
    cerr << "MultiDataPlot<Type>::plot call Type::prepare" << endl;

  the_plot->prepare (data);

  if (verbose)
    cerr << "MultiDataPlot<Type>::plot ptr=" << the_plot.ptr() << endl;

  plots.push_back (the_plot);
  data_sets.push_back (data);

  Plot* like = the_plot->get_constructor()->construct();
  Type* new_plot = dynamic_cast<Type*> (like);

  if (!new_plot)
    throw Error (InvalidState, "Pulsar::MultiDataPlot<Type>::plot",
		 "Plot::Constructor::construct does not return a Type");

  // configure new_plot using the saved options
  for (unsigned i=0; i<options.size(); i++)
  {
    if (verbose)
      cerr << "Pulsar::MultiDataPlot<Type>::plot apply "
	"'" << options[i] << "'" << endl;
    new_plot->configure( options[i] );
  }

  // then replace the_plot with new_plot
  manage (new_plot);
}

template<class Type>
void Pulsar::MultiDataPlot<Type>::finalize ()
{
  if (verbose)
    cerr << "Pulsar::MultiDataPlot<Type>::finalize" << endl;

  if (!data_sets.size())
    return;

  Type* main_plot = plots[0];

  for (unsigned i=1; i < plots.size(); i++)
    main_plot->get_frame()->include( plots[i]->get_frame() );

  for (unsigned i=0; i < plots.size(); i++)
  {
    plots[i]->get_frame()->copy( main_plot->get_frame() );
    plots[i]->get_frame()->freeze();
  }

  for (unsigned i=0; i < plots.size(); i++)
    plots[i]->plot( data_sets[i] );

  plots.resize(0);
  data_sets.resize(0);
}


#endif

