/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalibratorSpectrum.h"

Pulsar::CalibratorSpectrum::Interface::Interface (CalibratorSpectrum* instance)
{
  if (instance)
    set_instance (instance);

  add( &CalibratorSpectrum::get_subint,
       &CalibratorSpectrum::set_subint,
       "subint", "Sub-integration to plot" );

  add( &CalibratorSpectrum::get_plot_Ip,
       &CalibratorSpectrum::set_plot_Ip,
       "Ip", "Plot total and polarized intensities" );

}
