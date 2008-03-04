//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/FluxCalibratorExtensionTI.C,v $
   $Revision: 1.4 $
   $Date: 2008/03/04 00:44:53 $
   $Author: nopeer $ */



#include "Pulsar/FluxCalibratorExtension.h"



using Pulsar::FluxCalibratorExtension;



FluxCalibratorExtension::Interface::Interface( FluxCalibratorExtension *s_instance )
{
  if( s_instance )
    set_instance ( s_instance );

  add( &FluxCalibratorExtension::get_nchan, "nchan", "Nr of frequency channels (I)" );
  add( &FluxCalibratorExtension::get_nreceptor, "nrcvr", "Number of receiver channels (I)" );
  add( &FluxCalibratorExtension::get_epoch, "epoch", "[MJD] Epoch of calibration obs" );
}





