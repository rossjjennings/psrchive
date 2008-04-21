/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/Backend.h"



using Pulsar::Backend;



Backend::Interface::Interface ( Backend *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

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




