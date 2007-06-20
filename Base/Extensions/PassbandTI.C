//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Attic/PassbandTI.C,v $
   $Revision: 1.1 $
   $Date: 2007/06/20 03:06:46 $
   $Author: nopeer $ */





#include "Pulsar/PassbandTI.h"



using namespace Pulsar;



PassbandTI::PassbandTI()
{
  setup();
}


PassbandTI::PassbandTI( Passband *c )
{
  setup();
  set_instance( c );
}


void PassbandTI::setup( void )
{
  add( &Passband::get_nchan, "nchan", "Number of channels in original bandpass" );
  add( &Passband::get_npol, "npol", "Number of polarizations in bandpass" );
}


TextInterface::Class *PassbandTI::clone()
{
  if( instance )
    return new PassbandTI( instance );
  else
    return new PassbandTI();
}

