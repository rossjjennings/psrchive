/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FeedExtensionTI.h"



using Pulsar::FeedExtension;



FeedExtension::Interface::Interface ( FeedExtension *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  // Currently there are not gettable/settable parameters for a FeedExtension
}




