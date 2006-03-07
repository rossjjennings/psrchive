#include "Pulsar/BackendTI.h"

Pulsar::BackendTI::BackendTI ()
{
  add( &Backend::get_argument,
       &Backend::set_argument,
       "phase", "Phase convention of backend" );
}

