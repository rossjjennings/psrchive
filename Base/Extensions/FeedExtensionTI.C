/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FeedExtensionTI.h"

Pulsar::FeedExtensionTI::FeedExtensionTI ()
{
  SetupMethods();
}


Pulsar::FeedExtensionTI::FeedExtensionTI( FeedExtension *c )
{
  SetupMethods();
  set_instance( c );
}


void Pulsar::FeedExtensionTI::SetupMethods( void )
{
}


TextInterface::Class *Pulsar::FeedExtensionTI::clone()
{
  if( instance )
    return new FeedExtensionTI( instance );
  else
    return new FeedExtensionTI();
}

