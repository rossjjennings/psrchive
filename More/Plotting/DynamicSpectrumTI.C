/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/DynamicSpectrum.h"
#include <pairutil.h>

Pulsar::DynamicSpectrum::Interface::Interface (DynamicSpectrum* instance)
{
  if (instance)
    set_instance (instance);

  add( &DynamicSpectrum::get_srange,
      &DynamicSpectrum::set_srange, 
      "srange", "Range of subints to plot" );

  add( &DynamicSpectrum::get_pol,
      &DynamicSpectrum::set_pol,
      "pol", "Polarization to plot" );

  import("cmap", pgplot::ColourMap::Interface(), 
      &DynamicSpectrum::get_colour_map);

}
