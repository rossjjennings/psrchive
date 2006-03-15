#include "Pulsar/PhaseVsPlotTI.h"
#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PlotScaleTI.h"

Pulsar::PhaseVsPlotTI::PhaseVsPlotTI (PhaseVsPlot* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( PhasePlotTI() );

  import( "z", PlotScaleTI(), &PhaseVsPlot::get_z_scale );

  import("cmap", pgplot::ColourMap::Interface(), &PhaseVsPlot::get_colour_map);

}
