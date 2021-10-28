/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiData.h"
#include "Pulsar/MultiDataPlot.h"
#include "Pulsar/SimplePlot.h"
#include "Pulsar/MultiPlot.h"

Pulsar::Plot* Pulsar::MultiData::factory (Plot* plot)
{
  SimplePlot* simple = dynamic_cast<SimplePlot*>(plot);
  if (simple)
  {
    if (verbose)
      cerr << "MultiData::factory plot is a SimplePlot" << endl;
    return new MultiDataPlot<SimplePlot> (simple);
  }

  MultiPlot* multi = dynamic_cast<MultiPlot*>(plot);
  if (multi)
  {
    if (verbose)
      cerr << "MultiData::factory plot is a MultiPlot" << endl;
    return new MultiDataPlot<MultiPlot> (multi);
  }

  if (verbose)
    cerr << "MultiData::factory plot cannot overlay" << endl;

  return plot;
}

//! Process the Archive as needed before calling plot
void Pulsar::MultiData::preprocess (Archive* archive)
{
  managed_plot->preprocess (archive);
}

//! Provide access to the plot attributes
Pulsar::PlotAttributes* Pulsar::MultiData::get_attributes ()
{
  return managed_plot->get_attributes ();
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::MultiData::get_interface ()
{
  return managed_plot->get_interface ();
}

//! Get the text interface to the frame attributes
TextInterface::Parser* Pulsar::MultiData::get_frame_interface ()
{
  return managed_plot->get_frame_interface ();
}

//! Process a configuration command and store it for later use
void Pulsar::MultiData::configure (const std::string& option)
{
  managed_plot->configure (option);
  options.push_back (option);
}

//! Set the pre-processor
void Pulsar::MultiData::set_preprocessor (Processor* processor)
{
  managed_plot->set_preprocessor (processor);
}

//! Get the pre-processor
Pulsar::Processor* Pulsar::MultiData::get_preprocessor ()
{
  return managed_plot->get_preprocessor ();
}

//! Return true if pre-processor has been set
bool Pulsar::MultiData::has_preprocessor () const
{
  return managed_plot->has_preprocessor ();
}

