#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/PhasePlot.h"
#include "Pulsar/PlotScale.h"

Pulsar::PhaseVsPlot::Interface::Interface (PhaseVsPlot* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( PhasePlot::Interface() );

  import( "z", PlotScale::Interface(), &PhaseVsPlot::get_z_scale );

  import("cmap", pgplot::ColourMap::Interface(), &PhaseVsPlot::get_colour_map);

}
