/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/IntegrationTI.h"
#include "Pulsar/PointingTI.h"

Pulsar::IntegrationTI::IntegrationTI ()
{
  add( &Integration::get_nchan, "nchan", "Number of frequency channels" );
  add( &Integration::get_npol,  "npol",  "Number of polarizations" );
  add( &Integration::get_nbin,  "nbin",  "Number of pulse phase bins" );

  add( &Integration::get_epoch,
       &Integration::set_epoch,
       "mjd", "Epoch (MJD)" );
 
  add( &Integration::get_duration,
       &Integration::set_duration,
       "duration", "Duration of integration (seconds)" );

  add( &Integration::get_folding_period,
       &Integration::set_folding_period, 
       "period", "Period at which data were folded (seconds)" );

  import( "point", Pulsar::PointingTI(),
	  (Pointing*(Integration::*)()) &Integration::get<Pointing> );


}

