/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SpectrumPlot.h"
#include "Pulsar/ArchiveStatistic.h"
#include "Pulsar/PowerSpectra.h"

Pulsar::SpectrumPlot::Interface::Interface (SpectrumPlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( "pen", PlotPen::Interface(), &SpectrumPlot::get_pen );

  add( &SpectrumPlot::get_bin,
       &SpectrumPlot::set_bin,
       "bin", "Phase bin to plot" );

  add( &SpectrumPlot::get_statistic,
       &SpectrumPlot::set_statistic,
       &ArchiveStatistic::get_interface,
       "stat", "Statistic computed for each channel" );

  add( &SpectrumPlot::get_expression,
       &SpectrumPlot::set_expression,
       "exp", "Expression to evaluate" );

  import( PowerSpectra::Interface() );
}
