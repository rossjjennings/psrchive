/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/ObsExtensionTI.h"



using Pulsar::ObsExtension;



ObsExtension::Interface::Interface ( ObsExtension *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &ObsExtension::get_observer, "observer", "Observer name(s)" );
  add( &ObsExtension::get_project_ID, "projid", "Project name" );
  add( &ObsExtension::get_telescope, "telescop", "Telescope name" );
}




