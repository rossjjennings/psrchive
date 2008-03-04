/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/PolnCalibratorExtension.h"



using Pulsar::PolnCalibratorExtension;



PolnCalibratorExtension::Interface::Interface( PolnCalibratorExtension *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &PolnCalibratorExtension::get_epoch, "epoch", "[MJD] Epoch of calibration obs" );
  add( &PolnCalibratorExtension::get_ncpar, "npar", "Number of coupling parameters" );
  add( &PolnCalibratorExtension::get_nchan, "nchan", "Nr of channels in Feed coupling data" );
}



