#include "Pulsar/SpectrumPlotTI.h"
#include "Pulsar/PowerSpectraTI.h"

Pulsar::SpectrumPlotTI::SpectrumPlotTI (SpectrumPlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &SpectrumPlot::get_bin,
       &SpectrumPlot::set_bin,
       "bin", "Phase bin to plot" );

  import( PowerSpectraTI() );
}
