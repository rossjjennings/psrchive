//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicCalSpectrum.h"
#include "Pulsar/DynamicSpectrum.h"

Pulsar::DynamicCalSpectrum::Interface::Interface( 
    DynamicCalSpectrum *instance )
{
  if( instance )
    set_instance( instance );

  import( DynamicSpectrum::Interface() );
}

