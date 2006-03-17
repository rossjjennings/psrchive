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

  add( &Archive::get_telescope_code,
       &Archive::set_telescope_code,
       "site", "Telescope tempo code" );

  add( &Archive::get_source,
       &Archive::set_source,
       "name", "Name of the source" );

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

#if 0
  //! Get the coordinates of the source
  virtual sky_coord get_coordinates () const = 0;
  //! Set the coordinates of the source
  virtual void set_coordinates (const sky_coord& coordinates) = 0;

  //! Get the state of the profile data
  virtual Signal::State get_state () const = 0;
  //! Set the state of the profile data
  virtual void set_state (Signal::State state) = 0;
  
  //! Get the scale in which flux density is measured
  virtual Signal::Scale get_scale () const = 0;
  //! Set the scale in which flux density is measured
  virtual void set_scale (Signal::Scale scale) = 0;
  
  //! Get the observation type (psr, cal)
  virtual Signal::Source get_type () const = 0;
  //! Set the observation type (psr, cal)
  virtual void set_type (Signal::Source type) = 0;

#endif


  import( "rcvr", Pulsar::ReceiverTI(), 
	  (Receiver*(Archive::*)()) &Archive::get<Receiver>);

  import( "be", Pulsar::BackendTI(),
	  (Backend*(Archive::*)()) &Archive::get<Backend> );

  import( "int", IntegrationTI(), 
	  (Integration*(Archive::*)(unsigned)) &Archive::get_Integration,
	  &Archive::get_nsubint );

}

