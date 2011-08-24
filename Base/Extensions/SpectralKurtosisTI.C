/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SpectralKurtosis.h"

using Pulsar::SpectralKurtosis;

SpectralKurtosis::Interface::Interface ( SpectralKurtosis *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &SpectralKurtosis::get_M,
         &SpectralKurtosis::set_M,
         "m", "Integration factor (samples)" );

  add( &SpectralKurtosis::get_excision_threshold,
         &SpectralKurtosis::set_excision_threshold,
         "nsig", "Excision Threshold (stddevs)" );

}




