


#include "Pulsar/ObsExtensionTI.h"
#include <iostream>


using namespace std;


namespace Pulsar
{
  ObsExtensionTI::ObsExtensionTI()
  {
    SetupMethods();
  }

  ObsExtensionTI::ObsExtensionTI( ObsExtension *c )
  {
    SetupMethods();
    set_instance( c );
  }

  void ObsExtensionTI::SetupMethods( void )
  {
    add( &ObsExtension::get_observer, "observer", "Observer name(s)" );
    add( &ObsExtension::get_project_ID, "projid", "Project name" );
    add( &ObsExtension::get_telescope, "telescop", "Telescope name" );
  }

  TextInterface::Parser *Pulsar::ObsExtensionTI::clone()
  {
    if( instance )
      return new ObsExtensionTI( instance );
    else
      return new ObsExtensionTI();
  }

}


