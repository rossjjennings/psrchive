/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ArchiveTI.h"

#include "Pulsar/ReceiverTI.h"
#include "Pulsar/BackendTI.h"
#include "Pulsar/IntegrationTI.h"

Pulsar::ArchiveTI::ArchiveTI ()
{
  add( &Archive::get_filename, "file",  "Name of the file" );

  add( &Archive::get_nbin,     "nbin",  "Number of pulse phase bins" );
  add( &Archive::get_nchan,    "nchan", "Number of frequency channels" );
  add( &Archive::get_npol,     "npol",  "Number of polarizations" );
  add( &Archive::get_nsubint,  "nsub",  "Number of sub-integrations" );

  add( &Archive::get_type,
       &Archive::set_type,
       "type", "Observation type (Pulsar, PolnCal, etc.)" );

  add( &Archive::get_telescope_code,
       &Archive::set_telescope_code,
       "site", "Telescope tempo code" );

  add( &Archive::get_source,
       &Archive::set_source,
       "name", "Name of the source" );

#if 0
  //! Get the coordinates of the source
  virtual sky_coord get_coordinates () const = 0;
  //! Set the coordinates of the source
  virtual void set_coordinates (const sky_coord& coordinates) = 0;

#endif

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
       &Archive::set_dedispersed,    \
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

  import( "rcvr", Pulsar::ReceiverTI(), 
	  (Receiver*(Archive::*)()) &Archive::get<Receiver> );

  import( "be", Pulsar::BackendTI(),
	  (Backend*(Archive::*)()) &Archive::get<Backend> );

  import( "int", IntegrationTI(), 
	  (Integration*(Archive::*)(unsigned)) &Archive::get_Integration,
	  &Archive::get_nsubint );

}

