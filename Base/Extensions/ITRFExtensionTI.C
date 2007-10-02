


#include "Pulsar/ITRFExtensionTI.h"



namespace Pulsar
{

  ITRFExtensionTI::ITRFExtensionTI()
  {
    SetupMethods();
  }

  ITRFExtensionTI::ITRFExtensionTI( ITRFExtension *c )
  {
    SetupMethods();
    set_instance( c );
  }

  void ITRFExtensionTI::SetupMethods( void )
  {
    add( &ITRFExtension::get_ant_x, "ant_x", "ITRF X coordinate." );
    add( &ITRFExtension::get_ant_y, "ant_y", "ITRF Y coordinate." );
    add( &ITRFExtension::get_ant_z, "ant_z", "ITRF Z coordinate." );
  }
  
  TextInterface::Parser *Pulsar::ITRFExtensionTI::clone()
  {
    if( instance )
      return new ITRFExtensionTI( instance );
    else
      return new ITRFExtensionTI();
  }
}


