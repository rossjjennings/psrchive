/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotLoop.h"
#include "Pulsar/Plot.h"
#include "Pulsar/Archive.h"

#include <cpgplot.h>

using namespace std;

//! Default constructor
Pulsar::PlotLoop::PlotLoop ()
{
  overlay = false;
}

//! Set the Plot to be executed
void Pulsar::PlotLoop::set_Plot (Plot* p)
{
  the_plot = p;
}

//! Set the Archive to be plotted
void Pulsar::PlotLoop::set_Archive (const Archive* a)
{
  archive = a;
}

//! Set the overlay flag
void Pulsar::PlotLoop::set_overlay (bool flag)
{
  overlay = flag;
}

bool Pulsar::PlotLoop::get_overlay () const
{
  return overlay;
}

//! Add an index over which to loop
void Pulsar::PlotLoop::add_index (TextIndex* index)
{
  index_stack.push( index );
}

// execute the plot for each index in the stack
void Pulsar::PlotLoop::plot ()
{
  plot (index_stack);
}

void Pulsar::PlotLoop::plot( std::stack< Reference::To<TextIndex> >& indeces )
{
  if( indeces.empty() )
  {
    // bottom of stack; just plot the archive
    if (!overlay)
      cpgpage ();

    if (Plot::verbose)
      cerr << "Pulsar::PlotLoop::plot plotting" << endl;

    the_plot->plot (archive);
    return;
  }

  Reference::To<TextIndex> index = indeces.top();
  indeces.pop();

  index->set_container( const_cast<Archive*>(archive.get())->get_interface() );

  PlotLabel* label = the_plot->get_attributes()->get_label_above();
  string current = label->get_centre();

  for (unsigned i=0; i<index->size(); i++) try
  {
    string index_command = index->get_index(i);

    if (Plot::verbose)
      cerr << "Pulsar::PlotLoop::plot " << index_command << endl;

    the_plot->configure( index_command );

    label->set_centre( current + " " + index_command );

    plot (indeces);

    label->set_centre( current );
  }
  catch (Error& error)
  {
    cerr << "Pulsar::PlotLoop::plot error plotting " << index->get_index(i) 
         << " of "
         << "\n\t" << archive->get_filename()
         << "\n\t" << error.get_message() << endl;

    label->set_centre( current );
  }

  indeces.push( index );
}

