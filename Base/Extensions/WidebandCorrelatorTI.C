



#include "Pulsar/WidebandCorrelatorTI.h"



namespace Pulsar
{

  WidebandCorrelatorTI::WidebandCorrelatorTI()
  {
    SetupMethods();
  }

  WidebandCorrelatorTI::WidebandCorrelatorTI( WidebandCorrelator *c )
  {
    SetupMethods();
    set_instance(c);
  }

  void WidebandCorrelatorTI::SetupMethods( void )
  {
    add( &WidebandCorrelator::get_config, "beconfig", "Backend Config file" );
    add( &WidebandCorrelator::get_nrcvr, "nrcvr", "Number of receiver channels" );
    add( &WidebandCorrelator::get_tcycle, "tcycle", "Get the correlator cycle time" );
  }

  TextInterface::Class *Pulsar::WidebandCorrelatorTI::clone()
  {
    if( instance )
      return new WidebandCorrelatorTI( instance );
    else
      return new WidebandCorrelatorTI();
  }
}


