#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PhaseScaleTI.h"

Pulsar::PhasePlotTI::PhasePlotTI (PhasePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( "x", PhaseScaleTI(), &PhasePlot::get_scale );
}
