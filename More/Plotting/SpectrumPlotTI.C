#include "Pulsar/SpectrumPlot.h"
#include "Pulsar/PowerSpectra.h"

Pulsar::SpectrumPlot::Interface::Interface (SpectrumPlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &SpectrumPlot::get_bin,
       &SpectrumPlot::set_bin,
       "bin", "Phase bin to plot" );

  import( PowerSpectra::Interface() );
}
