/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorExtensionInterface.h"

Pulsar::FluxCalibratorExtension::Interface::Interface
( FluxCalibratorExtension *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  // read-only: requires resize
  add( &FluxCalibratorExtension::get_nreceptor,
       "nrcvr", "Number of receiver channels" );

  import ( CalibratorExtension::Interface() );

  // read-only: requires resize
  // add( &FluxCalibratorExtension::get_nchan,
  //     "nchan", "Number of frequency channels" );

  add( &FluxCalibratorExtension::has_scale,
       "scale", "scale (1=Native 0=Reference)" );

  add( &FluxCalibratorExtension::get_epoch,
       &FluxCalibratorExtension::set_epoch,
       "mjd", "Epoch of calibration observation" );
}





