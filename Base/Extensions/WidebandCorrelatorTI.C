



#include "Pulsar/WidebandCorrelatorTI.h"



namespace Pulsar {

	WidebandCorrelatorTI::WidebandCorrelatorTI()
	{
		add( &WidebandCorrelator::get_config, "beconfig", "Backend Config file" );
		add( &WidebandCorrelator::get_nrcvr, "nrcvr", "Number of receiver channels" );
		add( &WidebandCorrelator::get_tcycle, "tcycle", "Get the correlator cycle time" );
	}
}


