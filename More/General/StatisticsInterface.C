/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/PhaseWeightInterface.h"

Pulsar::Statistics::Interface::Interface (Statistics* instance)
{
  if (instance)
    set_instance (instance);

  add( &Statistics::get_subint,
       &Statistics::set_subint,
       "subint", "Sub-integration index" );

  add( &Statistics::get_chan,
       &Statistics::set_chan,
       "chan", "Frequency channel index" );

  add( &Statistics::get_pol,
       &Statistics::set_pol,
       "pol", "Polarization index" );

  import ( "on", PhaseWeight::Interface(), 
	   &Statistics::get_onpulse );

  import ( "off", PhaseWeight::Interface(), 
	   &Statistics::get_baseline );

  add( &Statistics::get_snr,
       "snr", "Total signal-to-noise ratio" );

  add( &Statistics::get_nfnr,
       "nfnr", "Total noise-to-Fourier-noise ratio" );

  add( &Statistics::get_cal_ntrans,
       "ncal", "Count the number of CAL transitions" );

  add( &Statistics::get_2bit_dist,
       "2bitd", "Estimate the 2-bit distortion" );
}


