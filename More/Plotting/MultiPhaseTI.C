#include "Pulsar/MultiPhase.h"
#include "Pulsar/PhaseScale.h"

Pulsar::MultiPhase::Interface::Interface (MultiPhase* instance)
{
  if (instance)
    set_instance (instance);

  import( "x", PhaseScale::Interface(), &MultiPhase::get_scale );
}
