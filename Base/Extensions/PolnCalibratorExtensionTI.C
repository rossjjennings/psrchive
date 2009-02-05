/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnCalibratorExtension.h"

Pulsar::PolnCalibratorExtension::Interface::Interface
( PolnCalibratorExtension *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  // read-only: requires resize
  add( &PolnCalibratorExtension::get_nchan,
       "nchan", "Number of frequency channels" );

  // read-only: requires resize
  add( &PolnCalibratorExtension::get_ncpar,
       "npar", "Number of receiver parameters" );

  add( &PolnCalibratorExtension::get_epoch,
       &PolnCalibratorExtension::set_epoch,
       "mjd", "Epoch of calibration observation" );
}



