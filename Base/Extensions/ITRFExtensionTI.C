/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/ITRFExtensionTI.h"



using Pulsar::ITRFExtension;



ITRFExtension::Interface::Interface ( ITRFExtension *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &ITRFExtension::get_ant_x, "ant_x", "ITRF X coordinate." );
  add( &ITRFExtension::get_ant_y, "ant_y", "ITRF Y coordinate." );
  add( &ITRFExtension::get_ant_z, "ant_z", "ITRF Z coordinate." );
}



