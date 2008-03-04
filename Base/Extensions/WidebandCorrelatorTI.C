/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/WidebandCorrelator.h"



using Pulsar::WidebandCorrelator;



WidebandCorrelator::Interface::Interface( WidebandCorrelator *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &WidebandCorrelator::get_config, "beconfig", "Backend Config file" );
  add( &WidebandCorrelator::get_nrcvr, "nrcvr", "Number of receiver channels" );
  add( &WidebandCorrelator::get_tcycle, "tcycle", "Get the correlator cycle time" );
}




