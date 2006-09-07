/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BackendTI.h"

Pulsar::BackendTI::BackendTI ()
{
  add( &Backend::get_argument,
       &Backend::set_argument,
       "phase", "Phase convention of backend" );

  add( &Backend::get_downconversion_corrected,
       &Backend::set_downconversion_corrected,
       "dcc", "Downconversion conjugation corrected" );
}

