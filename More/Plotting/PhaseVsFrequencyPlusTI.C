#include "Pulsar/PhaseVsFrequencyPlusTI.h"
#include "Pulsar/PhaseVsFrequencyTI.h"
//#include "Pulsar/SpectrumPlotTI.h"
#include "Pulsar/ProfilePlotTI.h"

Pulsar::PhaseVsFrequencyPlusTI::PhaseVsFrequencyPlusTI (PhaseVsFrequencyPlus* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsFrequencyPlus::get_subint,
       &PhaseVsFrequencyPlus::set_subint,
       "subint", "Sub-integration to plot" );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "freq", PhaseVsFrequencyTI(), &PhaseVsFrequencyPlus::get_freq );
  import ( "flux", ProfilePlotTI(), &PhaseVsFrequencyPlus::get_flux );
  // import ( "psd", SpectrumPlotTI(), &PhaseVsFrequencyPlus::get_psd );
}
