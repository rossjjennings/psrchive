#include "Pulsar/BackendTI.h"

Pulsar::BackendTI::BackendTI ()
{
  init ();
}

void Pulsar::BackendTI::init ()
{
  add( &Backend::get_argument,
       &Backend::set_argument,
       "phase", "Phase convention of backend" );
}

