/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/FluxPlot.h"

Pulsar::ProfilePlot::Interface::Interface (ProfilePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlot::Interface() );

  import ( "pen", PlotPen::Interface(), &ProfilePlot::get_pen );

  add( &ProfilePlot::get_plot_cal_transitions,
       &ProfilePlot::set_plot_cal_transitions,
       "cal", "Plot cal transitions (when applicable)" );

  add( &ProfilePlot::get_outlier_threshold,
       &ProfilePlot::set_outlier_threshold,
       "cut", "Threshold used to reject cal level outliers" );
}
