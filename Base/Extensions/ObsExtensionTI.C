


#include "Pulsar/ObsExtensionTI.h"



namespace Pulsar {

	ObsExtensionTI::ObsExtensionTI() {
	
		add( &ObsExtension::get_observer, "observer", "Observer" );
		add( &ObsExtension::get_project_ID, "projid", "Project ID" );
		add( &ObsExtension::get_telescope, "telescop", "Observer" );
	}

}


