#include "Pulsar/PhaseVsPlotterTI.h"
#include "Pulsar/ProfilePlotterTI.h"
#include "Pulsar/PlotZoomTI.h"

Pulsar::PhaseVsPlotterTI::PhaseVsPlotterTI (PhaseVsPlotter* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( ProfilePlotterTI() );

  import( "z", PlotZoomTI(), &PhaseVsPlotter::get_z_zoom );

}
