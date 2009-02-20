/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorExtensionInterface.h"

Pulsar::PolnCalibratorExtension::Interface::Interface
( PolnCalibratorExtension *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  // read-only: requires resize
  add( &PolnCalibratorExtension::get_nparam,
       "nparam", "Number of receiver parameters" );

  import ( CalibratorExtension::Interface::Interface() );
}



