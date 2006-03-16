#include "Pulsar/PhasePlot.h"
#include "Pulsar/PhaseScale.h"

Pulsar::PhasePlot::Interface::Interface (PhasePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( "x", PhaseScale::Interface(), &PhasePlot::get_scale );
}
