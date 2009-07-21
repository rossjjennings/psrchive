//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"

Pulsar::DynamicBaselineSpectrum::Interface::Interface( 
    DynamicBaselineSpectrum *instance )
{
  if( instance )
    set_instance( instance );

  add( &DynamicBaselineSpectrum::get_use_variance,
      &DynamicBaselineSpectrum::set_use_variance,
      "var", "Plot off-pulse variance rather than mean");

  import( DynamicSpectrum::Interface() );
}

