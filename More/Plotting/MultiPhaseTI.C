#include "Pulsar/MultiPhaseTI.h"
#include "Pulsar/PhaseScaleTI.h"

Pulsar::MultiPhaseTI::MultiPhaseTI (MultiPhase* instance)
{
  if (instance)
    set_instance (instance);

  import( "x", PhaseScaleTI(), &MultiPhase::get_scale );
}
