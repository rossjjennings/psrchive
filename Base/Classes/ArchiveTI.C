/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ArchiveTI.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"
#include "Pulsar/IntegrationTI.h"

#include "Pulsar/FITSAlias.h"

Pulsar::ArchiveTI::ArchiveTI ()
{
  setup();
}

Pulsar::ArchiveTI::ArchiveTI( Archive *c )
{
  setup();
  set_instance( c );
}

void Pulsar::ArchiveTI::setup( void )
{
  add( &Archive::get_filename, "file",    "Name of the file" );

  add( &Archive::get_nbin,     "nbin",    "Number of pulse phase bins" );
  add( &Archive::get_nchan,    "nchan",   "Number of frequency channels" );
  add( &Archive::get_npol,     "npol",    "Number of polarizations" );
  add( &Archive::get_nsubint,  "nsubint", "Number of sub-integrations" );

  add( &Archive::get_type,
       &Archive::set_type,
       "type", "Observation type (Pulsar, PolnCal, etc.)" );

  add( &Archive::get_telescope,
       &Archive::set_telescope,
       "site", "Telescope name" );

  add( &Archive::get_source,
       &Archive::set_source,
       "name", "Name of the source" );

  add( &Archive::get_coordinates,
       &Archive::set_coordinates,
       "coord", "Coordinates of the source" );

  add( &Archive::get_centre_frequency,
       &Archive::set_centre_frequency,
       "freq", "Centre frequency (MHz)" );

  add( &Archive::get_bandwidth,
       &Archive::set_bandwidth,
       "bw", "Bandwidth (MHz)" );

  add( &Archive::get_dispersion_measure,
       &Archive::set_dispersion_measure,
       "dm", "Dispersion measure (pc/cm^3)" );

  add( &Archive::get_rotation_measure,
       &Archive::set_rotation_measure,
       "rm", "Rotation measure (rad/m^2)" );

  add( &Archive::get_dedispersed,
       &Archive::set_dedispersed,
       "dmc", "Dispersion corrected (boolean)");

  add( &Archive::get_faraday_corrected,
       &Archive::set_faraday_corrected,
       "rmc", "Faraday Rotation corrected (boolean)" );

  add( &Archive::get_poln_calibrated,
       &Archive::set_poln_calibrated,
       "polc", "Polarization calibrated (boolean)" );

  add( &Archive::get_scale,
       &Archive::set_scale,
       "scale", "Units of profile amplitudes" );

  add( &Archive::get_state,
       &Archive::set_state,
       "state", "State of profile amplitudes" );
  
  add( &Archive::integration_length,
       "length", "The full duration of the observation (s)" );
  
  import( "rcvr", Pulsar::Receiver::Interface(),
          (Receiver*(Archive::*)()) &Archive::get<Receiver> );

  import( "be", Pulsar::Backend::Interface(),
          (Backend*(Archive::*)()) &Archive::get<Backend> );

  import( "int", IntegrationTI(),
          (Integration*(Archive::*)(unsigned)) &Archive::get_Integration,
          &Archive::get_nsubint );

  set_aliases( new FITSAlias );

}


TextInterface::Parser *Pulsar::ArchiveTI::clone()
{
  if( instance )
    return new ArchiveTI( instance );
  else
    return new ArchiveTI();
}

