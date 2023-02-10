/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotLoop.h"
#include "Pulsar/MultiPlot.h"
#include "Pulsar/MultiData.h"

#include "Pulsar/Plot.h"
#include "Pulsar/HasPen.h"

#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/Processor.h"

#include <cpgplot.h>

#define VERBOSE(x) if (Plot::verbose) std::cerr << x << std::endl

// #define VERBOSE(x) cerr << x << endl

using namespace std;

//! Default constructor
Pulsar::PlotLoop::PlotLoop ()
{
  preprocess = true;
  overlay = false;
  stack = false;

  current_colour_index = 0;
}

//! Add a Plot to be executed
void Pulsar::PlotLoop::add_Plot (Plot* p)
{
  VERBOSE("PlotLoop::add_Plot ptr=" << p);
  plots.push_back (p);
}

void Pulsar::PlotLoop::configure (const std::vector<std::string>& options)
{
  if (overlay)
    for (unsigned i=0; i<plots.size(); i++)
    {
      void* old_ptr = plots[i].ptr();
      plots[i] = MultiData::factory (plots[i]);
      VERBOSE("PlotLoop::configure ptr=" << old_ptr << "->" << plots[i].ptr());
    }

  if (stack && plots.size())
  {
    VERBOSE("PlotLoop::configure stack");
    MultiPlot* plot = MultiPlot::factory (plots[0]);
    for (unsigned i=0; i<plots.size(); i++)
      plot->manage (plots[i]);

    plots.resize(1);
    plots[0] = plot;
  }

  for (unsigned iopt=0; iopt < options.size(); iopt++)
    for (unsigned iplot=0; iplot < plots.size(); iplot++)
      plots[iplot]->configure( options[iopt] );
}

void Pulsar::PlotLoop::finalize ()
{
  for (unsigned i=0; i<plots.size(); i++)
  {
    VERBOSE("PlotLoop::finalize iplot=" << i);
    plots[i]->finalize ();
  }
}

//! Set the Archive to be plotted
void Pulsar::PlotLoop::set_Archive (Archive* a)
{
  archives.resize( plots.size() );

  current_colour_index = 0;

  for (unsigned iplot=0; iplot < plots.size(); iplot++)
  {
    archives[iplot] = a;

    if (preprocess || plots[iplot]->has_preprocessor())
      archives[iplot] = a->clone();

    if (preprocess)
      plots[iplot]->preprocess (archives[iplot]);

    if (plots[iplot]->has_preprocessor())
    {
      Processor* processor = plots[iplot]->get_preprocessor();
      processor->process (archives[iplot]);
      if (processor->result())
	archives[iplot] = processor->result();
    }
  }
}

void Pulsar::PlotLoop::set_overlay (bool flag)
{
  overlay = flag;
}

bool Pulsar::PlotLoop::get_overlay () const
{
  return overlay;
}

void Pulsar::PlotLoop::set_stack (bool flag)
{
  stack = flag;
}

bool Pulsar::PlotLoop::get_stack () const
{
  return stack;
}
void Pulsar::PlotLoop::set_preprocess (bool flag)
{
  preprocess = flag;
}

bool Pulsar::PlotLoop::get_preprocess () const
{
  return preprocess;
}

void Pulsar::PlotLoop::set_colour_indeces (const std::vector<unsigned>& ci)
{
  colour_indeces = ci;
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

// set the colour for the plot
void Pulsar::PlotLoop::set_colour_index (Plot* plot, unsigned colour_index)
{
  VERBOSE("PlotLoop::set_colour_index " << colour_index);

  MultiData* multi = dynamic_cast<MultiData*> (plot);
  if (multi)
  {
    VERBOSE("PlotLoop::set_colour_index Plot implements MultiData");
    set_colour_index (multi->get_Plot(), colour_index);
    return;
  }

  HasPen* has_pen = dynamic_cast<HasPen*> (plot);
  if (has_pen)
  {
    VERBOSE("PlotLoop::set_colour_index Plot implements HasPen");
    has_pen->get_pen()->set_colour_index (colour_index);
    return;
  }

  TextInterface::Parser* parser = plot->get_interface ();
  if (parser)
  {
    bool throw_exception = false;
    TextInterface::Value* value = parser->find ("sci", throw_exception);
    if (value)
    {
      VERBOSE("PlotLoop::set_colour_index Plot Interface has 'sci'");
      value->set_value (tostring(colour_index));
      return;
    }
  }

  VERBOSE("PlotLoop::set_colour_index cpgsci");
  cpgsci (colour_index);
}

void Pulsar::PlotLoop::plot( std::stack< Reference::To<TextIndex> >& indeces )
{
  if (indeces.empty())
  {
    VERBOSE("Pulsar::PlotLoop::plot plotting");

    for (unsigned i=0; i<plots.size(); i++)
    {
      if (!overlay)
        cpgpage ();

      VERBOSE("PlotLoop::plot iplot="<< i <<" ptr="<< (void*) plots[i].ptr());

      if (colour_indeces.size())
      {
        unsigned ci = colour_indeces[current_colour_index];
        current_colour_index ++;

        // cycle through colours if there are more iterations than indeces
        if (current_colour_index >= colour_indeces.size())
          current_colour_index = 0;

        set_colour_index (plots[i], ci);
      }

      plots[i]->plot (archives[i]);

      cpgsci (1);
    }
    return;
  }

  Reference::To<TextIndex> index = indeces.top();
  indeces.pop();

  vector<string> current (plots.size());

  unsigned loop_size = 0;
  for (unsigned iplot=0; iplot < plots.size(); iplot++)
  {
    index->set_container( archives[iplot]->get_interface() );

    if (iplot == 0)
      loop_size = index->size();
    else if ( loop_size != index->size() )
      throw Error (InvalidState, "Pulsar::PlotLoop::plot",
                   "loop size for plot[0]=%u != that of plot[%u]=%u",
                   loop_size, iplot, index->size());
  }
  
  Reference::To<TextInterface::Parser> interface;

  for (unsigned i=0; i<loop_size; i++)
  {
    for (unsigned iplot=0; iplot < plots.size(); iplot++) try
    {
      index->set_container( archives[iplot]->get_interface() );

      string index_command = index->get_index(i);

      VERBOSE("Pulsar::PlotLoop::plot " << index_command);

      if (!overlay)
      {
	PlotLabel* label = plots[iplot]->get_attributes()->get_label_above();
	current[iplot] = label->get_centre();
	if (current[iplot] != "unset")
	  label->set_centre( current[iplot] + " " + index_command );
      }

      //
      // by not calling Plot::configure, we avoid MultiData::configure,
      // which will add each of these temporary options to its common list
      //
      interface = plots[iplot]->get_interface();
      interface->process( index_command );
  
    }
    catch (Error& error)
    {
      cerr << "Pulsar::PlotLoop::plot error configuring "
	   << index->get_index(i) << " of "
           << "\n\t" << archives[iplot]->get_filename()
           << "\n\t" << error.get_message() << endl;
    }

    try
    {
      plot (indeces);
    }
    catch (Error& error)
    {
      cerr << "Pulsar::PlotLoop::plot error plotting " 
           << "\n\t" << error.get_message() << endl;
    }

    if (!overlay)
      for (unsigned iplot=0; iplot < plots.size(); iplot++)
      {
	PlotLabel* label = plots[iplot]->get_attributes()->get_label_above();
	label->set_centre( current[iplot] );
      }
  }

  indeces.push( index );
}

