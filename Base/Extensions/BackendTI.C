/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BackendTI.h"

Pulsar::BackendTI::BackendTI ()
{
  SetupMethods();
}


Pulsar::BackendTI::BackendTI( Backend *c )
{
  SetupMethods();
  set_instance( c );
}


void Pulsar::BackendTI::SetupMethods( void )
{
  add( &Backend::get_name,
         &Backend::set_name,
         "name", "Name of the backend instrument" );

  add( &Backend::get_argument,
         &Backend::set_argument,
         "phase", "Phase convention of backend" );

  add( &Backend::get_downconversion_corrected,
         &Backend::set_downconversion_corrected,
         "dcc", "Downconversion conjugation corrected" );
}

TextInterface::Class *Pulsar::BackendTI::clone()
{
  if( instance )
    return new BackendTI( instance );
  else
    return new BackendTI();
}

