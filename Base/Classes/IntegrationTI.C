#include "Pulsar/IntegrationTI.h"

Pulsar::IntegrationTI::IntegrationTI ()
{
  init ();
}

void Pulsar::IntegrationTI::init ()
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
}

