#include "Pulsar/PhaseVsPlotTI.h"
#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PlotZoomTI.h"

Pulsar::PhaseVsPlotTI::PhaseVsPlotTI (PhaseVsPlot* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( PhasePlotTI() );

  import( "z", PlotZoomTI(), &PhaseVsPlot::get_z_zoom );

}
