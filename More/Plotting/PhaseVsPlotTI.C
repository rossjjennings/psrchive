#include "Pulsar/PhaseVsPlotterTI.h"
#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PlotZoomTI.h"

Pulsar::PhaseVsPlotterTI::PhaseVsPlotterTI (PhaseVsPlotter* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( PhasePlotTI() );

  import( "z", PlotZoomTI(), &PhaseVsPlotter::get_z_zoom );

}
