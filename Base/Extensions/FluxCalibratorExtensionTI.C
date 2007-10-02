//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/FluxCalibratorExtensionTI.C,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 04:50:09 $
   $Author: straten $ */





#include "Pulsar/FluxCalibratorExtensionTI.h"



using namespace Pulsar;



FluxCalibratorExtensionTI::FluxCalibratorExtensionTI()
{
  setup();
}


FluxCalibratorExtensionTI::FluxCalibratorExtensionTI( FluxCalibratorExtension *c )
{
  setup();
  set_instance( c );
}


void FluxCalibratorExtensionTI::setup( void )
{
  add( &FluxCalibratorExtension::get_nchan, "nchan", "Nr of frequency channels (I)" );
  add( &FluxCalibratorExtension::get_nreceptor, "nrcvr", "Number of receiver channels (I)" );
  add( &FluxCalibratorExtension::get_epoch, "epoch", "[MJD] Epoch of calibration obs" );
}


TextInterface::Parser *FluxCalibratorExtensionTI::clone()
{
  if( instance )
    return new FluxCalibratorExtensionTI( instance );
  else
    return new FluxCalibratorExtensionTI();
}


