//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicSNSpectrum.h"
#include "Pulsar/DynamicSpectrum.h"

Pulsar::DynamicSNSpectrum::Interface::Interface( DynamicSNSpectrum *instance )
{
  if( instance )
    set_instance( instance );

  import( DynamicSpectrum::Interface() );
}

