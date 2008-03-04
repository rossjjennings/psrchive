/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/Passband.h"



using Pulsar::Passband;



Passband::Interface::Interface ( Passband *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &Passband::get_nchan, "nchan", "Number of channels in original bandpass" );
  add( &Passband::get_npol, "npol", "Number of polarizations in bandpass" );
}




