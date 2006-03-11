#include "Pulsar/PhaseVsPlotterTI.h"
#include "Pulsar/ProfilePlotterTI.h"
#include "Pulsar/PlotAxisTI.h"

Pulsar::PhaseVsPlotterTI::PhaseVsPlotterTI (PhaseVsPlotter* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( ProfilePlotterTI() );

  import( "z", PlotAxisTI(), &PhaseVsPlotter::get_z_axis );

  remove( "z:lab" );
  remove( "z:alt" );
  remove( "z:buf" );
  remove( "z:opt" );
}
