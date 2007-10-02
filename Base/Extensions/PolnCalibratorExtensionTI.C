/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnCalibratorExtensionTI.h"

Pulsar::PolnCalibratorExtensionTI::PolnCalibratorExtensionTI ()
{
  SetupMethods();
}


Pulsar::PolnCalibratorExtensionTI::PolnCalibratorExtensionTI( PolnCalibratorExtension *c )
{
  SetupMethods();
  set_instance( c );
}


void Pulsar::PolnCalibratorExtensionTI::SetupMethods( void )
{
  add( &PolnCalibratorExtension::get_epoch, "epoch", "[MJD] Epoch of calibration obs" );
  add( &PolnCalibratorExtension::get_ncpar, "npar", "Number of coupling parameters" );
  add( &PolnCalibratorExtension::get_nchan, "nchan", "Nr of channels in Feed coupling data" );
}

TextInterface::Parser *Pulsar::PolnCalibratorExtensionTI::clone()
{
  if( instance )
    return new PolnCalibratorExtensionTI( instance );
  else
    return new PolnCalibratorExtensionTI();
}

