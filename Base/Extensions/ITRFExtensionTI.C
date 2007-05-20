


#include "Pulsar/ITRFExtensionTI.h"



namespace Pulsar {

	ITRFExtensionTI::ITRFExtensionTI()
	{
		add( &ITRFExtension::get_ant_x, "ant_x", "ITRF X coordinate." );
		add( &ITRFExtension::get_ant_y, "ant_y", "ITRF Y coordinate." );
		add( &ITRFExtension::get_ant_z, "ant_z", "ITRF Z coordinate." );
	}
}


